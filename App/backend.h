#pragma once
#include <QObject>
#include <QUrl>
#include <QString>
#include <memory>
#include <QSettings>
#include <QVariantMap>
#include "foldermodel.h"

class Backend: public QObject
{
    Q_OBJECT
    Q_PROPERTY(FolderModel* srcModel READ srcModel CONSTANT)
    Q_PROPERTY(FolderModel* dstModel READ dstModel CONSTANT)

public:
    explicit Backend(QObject *parent=nullptr);
    explicit Backend(std::unique_ptr<QSettings> setting);

    Q_INVOKABLE auto toLocalPath(const QUrl &url)const ->QString;

    Q_INVOKABLE auto get8kPath() const -> QVariantMap;
    Q_INVOKABLE auto get16kPath() const -> QVariantMap;

    FolderModel* srcModel() const;
    FolderModel* dstModel() const;

private:
    std::unique_ptr<QSettings> m_setting{nullptr};
    FolderModel* m_srcModel;
    FolderModel* m_dstModel;
};

