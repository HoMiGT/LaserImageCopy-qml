#pragma once
#include <QObject>
#include <QUrl>
#include <QString>
#include <memory>
#include <QSettings>
#include <QVariantMap>

class Backend: public QObject
{
    Q_OBJECT


public:
    explicit Backend(QObject *parent=nullptr);
    explicit Backend(std::unique_ptr<QSettings> setting);

    Q_INVOKABLE auto toLocalPath(const QUrl &url)const ->QString;

    Q_INVOKABLE auto get8kPath() const -> QVariantMap;
    Q_INVOKABLE auto get16kPath() const -> QVariantMap;

private:
    std::unique_ptr<QSettings> m_setting{nullptr};
};

