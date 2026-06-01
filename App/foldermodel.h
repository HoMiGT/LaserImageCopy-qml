#pragma once

#include <QAbstractListModel>
#include <QList>

struct FolderData{
    QString folderName;
    int sumCount;
    int currentCount;
    int id;
};

class FolderModel : public QAbstractListModel
{
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
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    // 添加假数据
    void loadDummyData();

private:
    QList<FolderData> m_folderData;
};

