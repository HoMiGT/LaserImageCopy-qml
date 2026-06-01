#include "backend.h"
#include <QDebug>
#include <QFile>

Backend::Backend(QObject *parent)
    :QObject(parent), m_srcModel(new FolderModel(this)), m_dstModel(new FolderModel(this))
{
    m_srcModel->loadDummyData();
    m_dstModel->loadDummyData();
}

Backend::Backend(std::unique_ptr<QSettings> setting)
    :m_setting(std::move(setting)), m_srcModel(new FolderModel(this)), m_dstModel(new FolderModel(this))
{
    m_srcModel->loadDummyData();
    m_dstModel->loadDummyData();
}

auto Backend::toLocalPath(const QUrl &url)const ->QString{
    if (url.isEmpty()) return "";
    return url.toLocalFile();
}

auto Backend::get8kPath() const -> QVariantMap
{
    auto p1 = m_setting->value("Camera8K/SrcPath").toString();
    auto p2 = m_setting->value("Camera8K/DstPath").toString();
    QVariantMap map;
    map["src"] = p1;
    map["dst"] = p2;
    return map;
}

auto Backend::get16kPath() const -> QVariantMap
{
    auto p1 = m_setting->value("Camera16K/SrcPath").toString();
    auto p2 = m_setting->value("Camera16K/DstPath").toString();
    QVariantMap map;
    map["src"] = p1;
    map["dst"] = p2;
    return map;
}

FolderModel* Backend::srcModel() const {
    return m_srcModel;
}

FolderModel* Backend::dstModel() const {
    return m_dstModel;
}
