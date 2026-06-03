#pragma once

#include <QAbstractListModel>
#include <QList>

struct FolderData {
  QString folderName;
  int sumCount;
  int currentCount;
  int id;
};

class FolderModel : public QAbstractListModel {
  Q_OBJECT

public:
  enum FolderRoles {
    FolderNameRole = Qt::UserRole + 1,
    SumCountRole,
    CurrentCountRole,
    IdRole
  };

  explicit FolderModel(QObject *parent = nullptr);

  int rowCount(const QModelIndex &parent = QModelIndex()) const override;
  QVariant data(const QModelIndex &index,
                int role = Qt::DisplayRole) const override;
  QHash<int, QByteArray> roleNames() const override;

  // 动态更新数据
  void setFolderData(const QList<FolderData> &data);

  // 留给 backend 的进度更新入口，只更新单项数据，防止整个列表刷新
  Q_INVOKABLE void updateItemProgress(int row, int currentCount);
  Q_INVOKABLE void updateItemProgressByName(const QString &folderName, int currentCount);

private:
  QList<FolderData> m_folderData;
};
