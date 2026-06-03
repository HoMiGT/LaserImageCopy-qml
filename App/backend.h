#pragma once
#include "foldermodel.h"
#include <QObject>
#include <QSettings>
#include <QString>
#include <QUrl>
#include <QVariantMap>
#include <memory>

class Backend : public QObject {
  Q_OBJECT
  Q_PROPERTY(FolderModel *srcModel READ srcModel CONSTANT)
  Q_PROPERTY(FolderModel *dstModel READ dstModel CONSTANT)

signals:
  void pathNotExist(const QString &kind, const QString &path);
  void copyFinished(const QString &message);

public:
  explicit Backend(QObject *parent = nullptr);
  explicit Backend(std::unique_ptr<QSettings> setting);

  Q_INVOKABLE auto toLocalPath(const QUrl &url) const -> QString;

  Q_INVOKABLE auto get8kPath() const -> QVariantMap;
  Q_INVOKABLE auto get16kPath() const -> QVariantMap;

  Q_INVOKABLE void startCopy(const QString &srcPath, const QString &dstPath,
                             const QString &cameraType, bool isVConcat);

  Q_INVOKABLE bool scanDirectory(const QString &path, const QString &cameraType,
                                 const QString &kind);

  FolderModel *srcModel() const;
  FolderModel *dstModel() const;

private:
  std::unique_ptr<QSettings> m_setting{nullptr};
  FolderModel *m_srcModel;
  FolderModel *m_dstModel;
  QList<FolderData> m_dataList;
};
