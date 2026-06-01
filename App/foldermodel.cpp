#include "foldermodel.h"

FolderModel::FolderModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

int FolderModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_folderData.size();
}

QVariant FolderModel::data(const QModelIndex &index, int role) const
{
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

QHash<int, QByteArray> FolderModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[FolderNameRole] = "folderName";
    roles[SumCountRole] = "sumCount";
    roles[CurrentCountRole] = "currentCount";
    roles[IdRole] = "id";
    return roles;
}

void FolderModel::loadDummyData()
{
    beginResetModel();
    m_folderData.clear();
    
    // 动态生成 25 条模拟数据
    for (int i = 1; i <= 25; ++i) {
        int sumCount = 100 + (i * 15);
        int currentCount = (i % 4 == 0) ? sumCount : (sumCount / (i % 3 + 1.5));
        QString folderName = QString("LaserImage_Batch_%1").arg(i, 3, 10, QChar('0'));
        
        m_folderData.append({folderName, sumCount, currentCount, i});
    }
    
    endResetModel();
}
