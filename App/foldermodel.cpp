#include "foldermodel.h"

FolderModel::FolderModel(QObject *parent) : QAbstractListModel(parent) {}

int FolderModel::rowCount(const QModelIndex &parent) const {
  if (parent.isValid())
    return 0;
  return m_folderData.size();
}

QVariant FolderModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid() || index.row() >= m_folderData.size() || index.row() < 0)
    return QVariant();

  const FolderData &folder = m_folderData[index.row()];
  switch (role) {
  case FolderNameRole:
    return folder.folderName;
  case SumCountRole:
    return folder.sumCount;
  case CurrentCountRole:
    return folder.currentCount;
  case IdRole:
    return folder.id;
  }
  return QVariant();
}

QHash<int, QByteArray> FolderModel::roleNames() const {
  QHash<int, QByteArray> roles;
  roles[FolderNameRole] = "folderName";
  roles[SumCountRole] = "sumCount";
  roles[CurrentCountRole] = "currentCount";
  roles[IdRole] = "id";
  return roles;
}

void FolderModel::setFolderData(const QList<FolderData> &data) {
  beginResetModel();
  m_folderData = data;
  endResetModel();
}

void FolderModel::updateItemProgress(int row, int currentCount) {
  if (row < 0 || row >= m_folderData.size())
    return;

  if (m_folderData[row].currentCount != currentCount) {
    m_folderData[row].currentCount = currentCount;
    QModelIndex idx = index(row, 0);
    emit dataChanged(idx, idx, {CurrentCountRole});
  }
}

void FolderModel::updateItemProgressByName(const QString &folderName,
                                           int currentCount) {
  for (int i = 0; i < m_folderData.size(); ++i) {
    if (m_folderData[i].folderName == folderName) {
      updateItemProgress(i, currentCount);
      return;
    }
  }
}
