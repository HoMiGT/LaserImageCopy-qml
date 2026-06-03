#include "backend.h"
#include <QDir>
#include <QElapsedTimer>
#include <QFile>
#include <QThreadPool>
#include <QtConcurrent>
#include <QImage>
#include <QPainter>
#include <atomic>
#include <memory>

Backend::Backend(QObject *parent)
    : QObject(parent), m_srcModel(new FolderModel(this)),
      m_dstModel(new FolderModel(this)) {}

Backend::Backend(std::unique_ptr<QSettings> setting)
    : m_setting(std::move(setting)), m_srcModel(new FolderModel(this)),
      m_dstModel(new FolderModel(this)) {}

auto Backend::toLocalPath(const QUrl &url) const -> QString {
  if (url.isEmpty())
    return "";
  return url.toLocalFile();
}

auto Backend::get8kPath() const -> QVariantMap {
  auto p1 = m_setting->value("Camera8K/SrcPath").toString();
  auto p2 = m_setting->value("Camera8K/DstPath").toString();
  QVariantMap map;
  map["src"] = p1;
  map["dst"] = p2;
  return map;
}

auto Backend::get16kPath() const -> QVariantMap {
  auto p1 = m_setting->value("Camera16K/SrcPath").toString();
  auto p2 = m_setting->value("Camera16K/DstPath").toString();
  QVariantMap map;
  map["src"] = p1;
  map["dst"] = p2;
  return map;
}

FolderModel *Backend::srcModel() const { return m_srcModel; }

FolderModel *Backend::dstModel() const { return m_dstModel; }

bool Backend::scanDirectory(const QString &path, const QString &cameraType,
                            const QString &kind) {
    if (path.isEmpty()){
        QList<FolderData> dataList;
        if (kind == "src")
            m_srcModel->setFolderData(dataList);
        else
            m_dstModel->setFolderData(dataList);
        return false;
    }

  QDir rootDir(path);
  if (!rootDir.exists()) {
    emit pathNotExist(kind, path);
    return false;
  }
  if (kind == "src") {
    m_dataList.clear();
    int idCounter = 1;

    if (cameraType == "custom") {
      if (!rootDir.absolutePath().endsWith("_copied")) {
        int fileCount = rootDir.entryList(QDir::Files).count();
        // 在自定义模式下，只显示源路径这一级。界面显示的 folderName
        // 为路径最后一部分。
        m_dataList.append(
            {rootDir.dirName(), fileCount, fileCount, idCounter++});
      }
    } else {
      // 获取第一级目录
      QStringList level1Dirs =
          rootDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
      for (const QString &l1 : level1Dirs) {
        QDir l1Dir(rootDir.filePath(l1));

        // 获取第二级目录
        QStringList level2Dirs =
            l1Dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);

        for (const QString &l2 : level2Dirs) {
          QDir l2Dir(l1Dir.filePath(l2));
          if (l2Dir.absolutePath().endsWith("_copied")) {
            continue;
          }
          // 统计第二级目录下的文件数量
          int fileCount =
              l2Dir.entryList(QDir::Files | QDir::NoDotAndDotDot).count();

          // 后两级目录拼接作为名称
          QString folderName = l1 + "/" + l2;

          // 当前将其进度视为满的 (总数量 = 当前数量 = fileCount)
          m_dataList.append({folderName, fileCount, fileCount, idCounter++});
        }
      }
    }
    m_srcModel->setFolderData(m_dataList);
  } else if (kind == "dst") {
    QList<FolderData> dataList;
    for (const auto &item : m_dataList) {
      dataList.append({item.folderName, item.sumCount, 0, item.id});
    }
    m_dstModel->setFolderData(dataList);
  }
  return true;
}

void Backend::startCopy(const QString &srcPath, const QString &dstPath,
                        const QString &cameraType, bool isVConcat) {
  if (srcPath.isEmpty() || dstPath.isEmpty())
    return;

  if (cameraType == "16k" && isVConcat) {
    QtConcurrent::run([this, srcPath, dstPath, cameraType]() {
      QElapsedTimer timer;
      timer.start();
      struct StitchTask {
        int k;
        QString srcDirPath;
        QString dstDirPath;
        QString folderName;
        QStringList files;
        int originalSumCount;
        int totalOutCount;
        std::shared_ptr<std::atomic<int>> countPtr;
        Backend *backend;
      };

      QList<StitchTask> tasks;

      for (const auto &folderData : m_dataList) {
        QString relPath = (cameraType == "custom") ? "" : folderData.folderName;
        QDir srcDir(srcPath + (relPath.isEmpty() ? "" : ("/" + relPath)));
        QDir dstDir(dstPath + (relPath.isEmpty() ? "" : ("/" + relPath)));

        if (!dstDir.exists()) {
          dstDir.mkpath(".");
        }

        QStringList files = srcDir.entryList(QDir::Files | QDir::NoDotAndDotDot);
        if (files.isEmpty()) continue;
        
        // 确保文件按顺序排列
        files.sort();

        int N = files.size();
        // 拼接后全局高度：每张图与前一张重叠 1/4 (2000px)，即向下移动 6000px。最后一张贡献完整的 8000px。
        int H_total = (N > 0) ? ((N - 1) * 6000 + 8000) : 0;
        int M = (H_total + 7999) / 8000;

        std::shared_ptr<std::atomic<int>> copiedCount = std::make_shared<std::atomic<int>>(0);

        for (int k = 0; k < M; ++k) {
          tasks.append({k, srcDir.absolutePath(), dstDir.absolutePath(),
                        folderData.folderName, files, folderData.sumCount, M, copiedCount, this});
        }
      }

      QtConcurrent::blockingMap(tasks, [](const StitchTask &task) {
        int N = task.files.size();
        int H_total = (N - 1) * 6000 + 8000;
        
        QImage outImg(8192, 8000, QImage::Format_RGB32);
        outImg.fill(Qt::black);
        QPainter painter(&outImg);

        int Y_start = task.k * 8000;
        int Y_end = std::min((task.k + 1) * 8000 - 1, H_total - 1);
        
        int Y_curr = Y_start;
        while (Y_curr <= Y_end) {
            int i = std::min(Y_curr / 6000, N - 1);
            int Y_block_end = Y_end;
            if (i < N - 1) {
                Y_block_end = std::min(Y_end, (i + 1) * 6000 - 1);
            }
            
            int y_local = Y_curr - i * 6000;
            int y_out = Y_curr - Y_start;
            int h = Y_block_end - Y_curr + 1;
            
            QString srcFilePath = task.srcDirPath + "/" + task.files[i];
            QImage srcImg(srcFilePath);
            
            painter.drawImage(0, y_out, srcImg, 0, y_local, 8192, h);
            
            Y_curr = Y_block_end + 1;
        }
        painter.end();
        
        // 保存图片
        QString outFileName = QString("stitched_%1.jpg").arg(task.k, 4, 10, QChar('0'));
        QString dstFilePath = task.dstDirPath + "/" + outFileName;
        outImg.save(dstFilePath, "JPG", 90);

        int current = ++(*task.countPtr);
        
        // 为了不改动折叠模型 (sumCount=N不变)，将输出张数等比例映射回源图片张数
        int scaledCurrent = (task.totalOutCount > 0) ? (current * task.originalSumCount / task.totalOutCount) : 0;
        if (current == task.totalOutCount) scaledCurrent = task.originalSumCount;
        
        // 目标路径：增加数量
        QMetaObject::invokeMethod(
            task.backend->dstModel(), "updateItemProgressByName",
            Qt::QueuedConnection, Q_ARG(QString, task.folderName),
            Q_ARG(int, scaledCurrent));

        // 源路径：减少数量
        int srcRemain = task.originalSumCount - scaledCurrent;
        QMetaObject::invokeMethod(
            task.backend->srcModel(), "updateItemProgressByName",
            Qt::QueuedConnection, Q_ARG(QString, task.folderName),
            Q_ARG(int, srcRemain));
      });

      // 拷贝完成后重命名源目录，添加 _copied 后缀
      for (const auto &folderData : m_dataList) {
        QString relPath = (cameraType == "custom") ? "" : folderData.folderName;
        QDir srcDir(srcPath + (relPath.isEmpty() ? "" : ("/" + relPath)));
        if (srcDir.exists()) {
          QString newPath = srcDir.absolutePath() + "_copied";
          QDir().rename(srcDir.absolutePath(), newPath);
        }
      }

      qint64 elapsed = timer.elapsed();
      QString msg = QString("拼接任务执行成功！\n总耗时: %1 秒").arg(elapsed / 1000.0, 0, 'f', 2);
      emit copyFinished(msg);
      
      qDebug() << "Stitch finished!";
    });
  } else {
    QtConcurrent::run([this, srcPath, dstPath, cameraType]() {
      QElapsedTimer timer;
      timer.start();
      struct CopyTask {
        QString srcFile;
        QString dstFile;
        QString folderName;
        int totalCount;
        std::shared_ptr<std::atomic<int>> countPtr;
        Backend *backend;
      };

      QList<CopyTask> tasks;

      for (const auto &folderData : m_dataList) {
        // 自定义模式下不需要追加子路径，本身就是在源路径下
        QString relPath = (cameraType == "custom") ? "" : folderData.folderName;

        QDir srcDir(srcPath + (relPath.isEmpty() ? "" : ("/" + relPath)));
        QDir dstDir(dstPath + (relPath.isEmpty() ? "" : ("/" + relPath)));

        if (!dstDir.exists()) {
          dstDir.mkpath(".");
        }

        QStringList files =
            srcDir.entryList(QDir::Files | QDir::NoDotAndDotDot);
        std::shared_ptr<std::atomic<int>> copiedCount =
            std::make_shared<std::atomic<int>>(0);

        for (const QString &f : files) {
          // 此时更新进度用的 folderName 依然保持界面显示的名称
          // (folderData.folderName)
          tasks.append({srcDir.absoluteFilePath(f), dstDir.absoluteFilePath(f),
                        folderData.folderName, folderData.sumCount, copiedCount,
                        this});
        }
      }

      QtConcurrent::blockingMap(tasks, [](const CopyTask &task) {
        if (QFile::exists(task.dstFile)) {
          QFile::remove(task.dstFile);
        }
        if (QFile::copy(task.srcFile, task.dstFile)) {
          int current = ++(*task.countPtr);

          // 目标路径：增加数量 (从 0 到 totalCount)
          QMetaObject::invokeMethod(
              task.backend->dstModel(), "updateItemProgressByName",
              Qt::QueuedConnection, Q_ARG(QString, task.folderName),
              Q_ARG(int, current));

          // 源路径：减少数量 (从 totalCount 到 0)
          int srcRemain = task.totalCount - current;
          QMetaObject::invokeMethod(
              task.backend->srcModel(), "updateItemProgressByName",
              Qt::QueuedConnection, Q_ARG(QString, task.folderName),
              Q_ARG(int, srcRemain));
        }
      });

      // 拷贝完成后重命名源目录，添加 _copied 后缀
      for (const auto &folderData : m_dataList) {
        QString relPath = (cameraType == "custom") ? "" :
        folderData.folderName; QDir srcDir(srcPath + (relPath.isEmpty() ? ""
        :
        ("/" + relPath))); if (srcDir.exists()) {
          QString newPath = srcDir.absolutePath() + "_copied";
          QDir().rename(srcDir.absolutePath(), newPath);
        }
      }

      qint64 elapsed = timer.elapsed();
      QString msg = QString("拷贝任务执行成功！\n总耗时: %1 秒").arg(elapsed / 1000.0, 0, 'f', 2);
      emit copyFinished(msg);

      qDebug() << "Copy finished!";
    });
  }
}
