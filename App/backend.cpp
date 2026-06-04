#include "backend.h"
#include <QDir>
#include <QElapsedTimer>
#include <QFile>
#include <QImage>
#include <QPainter>
#include <QThreadPool>
#include <QtConcurrent>
#include <atomic>
#include <memory>
#include <opencv2/opencv.hpp>
#include <string>
#include <vector>

using namespace cv;

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
  if (path.isEmpty()) {
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

std::vector<std::vector<std::string>> inline split_with_overlap(
    const std::vector<std::string> &input, const size_t n) {
  std::vector<std::vector<std::string>> result;
  if (n == 0 || input.empty())
    return result;
  result.reserve(n);
  const auto total = input.size();
  const auto base_size = static_cast<int>(std::ceil(static_cast<double>(total) /
                                                    static_cast<double>(n))) +
                         1;
  int start{0};
  int end{0};
  for (auto i{0}; i < n; ++i) {
    start = i == 0 ? i * base_size : i * (base_size - 1);
    end = start + base_size;
    if (end > total) {
      end = total;
      i = n; // 结束循环
    }
    result.emplace_back(input.begin() + start, input.begin() + end);
  }
  return result;
}

void Backend::startCopy(const QString &srcPath, const QString &dstPath,
                        const QString &cameraType, bool isVConcat) {
  if (srcPath.isEmpty() || dstPath.isEmpty())
    return;

  if (cameraType == "16k" && isVConcat) {
    QtConcurrent::run([this, srcPath, dstPath, cameraType]() {
      QElapsedTimer timer;
      timer.start();

      for (const auto &folderData : m_dataList) {
        QString relPath = (cameraType == "custom") ? "" : folderData.folderName;
        QDir srcDir(srcPath + (relPath.isEmpty() ? "" : ("/" + relPath)));
        QDir dstDir(dstPath + (relPath.isEmpty() ? "" : ("/" + relPath)));

        if (!dstDir.exists()) {
          dstDir.mkpath(".");
        }

        QStringList files =
            srcDir.entryList(QDir::Files | QDir::NoDotAndDotDot);
        if (files.isEmpty())
          continue;

        // 确保文件按顺序排列
        files.sort();

        std::vector<std::string> stdFiles;
        stdFiles.reserve(files.count());

        for (const auto &file : files) {
          stdFiles.emplace_back(file.toStdString());
        }
        auto splitFiles = split_with_overlap(stdFiles, 8);

        std::shared_ptr<std::atomic<int>> copiedCount =
            std::make_shared<std::atomic<int>>(0);
        int totalCount = files.count();

        struct SplitChunk {
          int v_idx;
          std::vector<std::string> chunkFiles;
        };

        QList<SplitChunk> chunks;
        for (size_t v = 0; v < splitFiles.size(); ++v) {
          chunks.append({static_cast<int>(v), splitFiles[v]});
        }

        QString srcDirPath = srcDir.absolutePath();
        QString dstDirPath = dstDir.absolutePath();
        QString folderName = folderData.folderName;

        QtConcurrent::blockingMap(chunks, [this, srcDirPath, dstDirPath,
                                           folderName, totalCount, copiedCount](
                                              const SplitChunk &chunk) {
          cv::Mat bottomPart;
          for (size_t i = 0; i < chunk.chunkFiles.size(); ++i) {
            QString srcFilePath =
                srcDirPath + "/" + QString::fromStdString(chunk.chunkFiles[i]);
            cv::Mat currImg = cv::imread(
                srcFilePath.toLocal8Bit().toStdString(), cv::IMREAD_COLOR);
            if (currImg.empty())
              continue;

            // 1. 先从原始当前图片中提取后 1/4 (比率计算)，供下一张使用
            int h = currImg.rows;
            int w = currImg.cols;
            int copyH = h / 4;
            int startY = h - copyH;
            cv::Mat nextBottomPart;
            if (copyH > 0 && startY >= 0 && startY < h) {
              nextBottomPart = currImg(cv::Rect(0, startY, w, copyH)).clone();
            }

            cv::Mat finalImg;
            // 2. 将前一张的后 1/4 拼接到当前张的正上方，总高度增加
            if (i > 0 && !bottomPart.empty()) {
              if (bottomPart.cols == currImg.cols &&
                  bottomPart.type() == currImg.type()) {
                // 如果宽度和类型一致，使用高效的垂直拼接
                cv::vconcat(bottomPart, currImg, finalImg);
              } else {
                // 如果不一致，按照最大宽度分配新图片空间进行拼接
                int maxW = std::max(bottomPart.cols, currImg.cols);
                int newH = bottomPart.rows + currImg.rows;
                finalImg = cv::Mat::zeros(newH, maxW, currImg.type());
                bottomPart.copyTo(
                    finalImg(cv::Rect(0, 0, bottomPart.cols, bottomPart.rows)));
                currImg.copyTo(finalImg(
                    cv::Rect(0, bottomPart.rows, currImg.cols, currImg.rows)));
              }
            } else {
              // 第一张图片不需要拼接
              finalImg = currImg;
            }

            // 更新 bottomPart 为下一张图准备
            bottomPart = nextBottomPart;

            // 3. 第1个vector之后的所有vector，都不需要保存第一张图片
            bool shouldSave = true;
            if (chunk.v_idx > 0 && i == 0) {
              shouldSave = false;
            }

            if (shouldSave) {
              QString dstFilePath = dstDirPath + "/" +
                                    QString::fromStdString(chunk.chunkFiles[i]);
              std::vector<int> compression_params = {cv::IMWRITE_JPEG_QUALITY,
                                                     90};
              // 注意这里保存的是最终拼接变长的 finalImg
              cv::imwrite(dstFilePath.toLocal8Bit().toStdString(), finalImg,
                          compression_params);

              int current = ++(*copiedCount);

              // 更新 UI 进度
              QMetaObject::invokeMethod(
                  dstModel(), "updateItemProgressByName", Qt::QueuedConnection,
                  Q_ARG(QString, folderName), Q_ARG(int, current));

              int srcRemain = totalCount - current;
              QMetaObject::invokeMethod(
                  srcModel(), "updateItemProgressByName", Qt::QueuedConnection,
                  Q_ARG(QString, folderName), Q_ARG(int, srcRemain));
            }
          }
        });
      }

      // 拷贝完成后重命名源目录，添加 _copied 后缀
      // for (const auto &folderData : m_dataList) {
      //   QString relPath = (cameraType == "custom") ? "" :
      //   folderData.folderName; QDir srcDir(srcPath + (relPath.isEmpty() ? ""
      //   : ("/" + relPath))); if (srcDir.exists()) {
      //     QString newPath = srcDir.absolutePath() + "_copied";
      //     QDir().rename(srcDir.absolutePath(), newPath);
      //   }
      // }

      qint64 elapsed = timer.elapsed();
      QString msg = QString("拼接任务执行成功！\n总耗时: %1 秒")
                        .arg(elapsed / 1000.0, 0, 'f', 2);
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
        QString relPath = (cameraType == "custom") ? "" : folderData.folderName;
        QDir srcDir(srcPath + (relPath.isEmpty() ? "" : ("/" + relPath)));
        if (srcDir.exists()) {
          QString newPath = srcDir.absolutePath() + "_copied";
          QDir().rename(srcDir.absolutePath(), newPath);
        }
      }

      qint64 elapsed = timer.elapsed();
      QString msg = QString("拷贝任务执行成功！\n总耗时: %1 秒")
                        .arg(elapsed / 1000.0, 0, 'f', 2);
      emit copyFinished(msg);

      qDebug() << "Copy finished!";
    });
  }
}
