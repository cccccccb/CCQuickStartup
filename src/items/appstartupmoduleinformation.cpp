#include "appstartupmoduleinformation.h"

#include "private/appstartupinstance_p.h"

#include <QQuickItem>

AppStartupModuleInformation::AppStartupModuleInformation()
    : m_startModule(UnknowModule)
    , m_default(false)
{

}

AppStartupModuleInformation::AppStartupModuleInformation(const QString &path)
{
    if (AppStartupInstancePrivate::resolveInformation(path, this)) {
        this->m_path = path;
    } else {
        qWarning() << "[App Startup] resolve information failed from path: " << path;
    }
}

AppStartupModuleInformation::AppStartupModuleInformation(const AppStartupModuleInformation &other)
{
    this->m_startModule = other.m_startModule;
    this->m_appId = other.m_appId;
    this->m_descriptor = other.m_descriptor;
    this->m_version = other.m_version;
    this->m_description = other.m_description;
    this->m_features = other.m_features;
    this->m_changelog = other.m_changelog;
    this->m_default = other.m_default;
    this->m_path = other.m_path;
}

AppStartupModuleInformation::AppStartupModuleInformation(AppStartupModuleInformation &&other)
{
    this->m_startModule = std::move(other.m_startModule);
    this->m_appId = std::move(other.m_appId);
    this->m_descriptor = std::move(other.m_descriptor);
    this->m_version = std::move(other.m_version);
    this->m_description = std::move(other.m_description);
    this->m_features = std::move(other.m_features);
    this->m_changelog = std::move(other.m_changelog);
    this->m_default = std::move(other.m_default);
    this->m_path = std::move(other.m_path);
}

AppStartupModuleInformation &AppStartupModuleInformation::operator=(const AppStartupModuleInformation &other)
{
    this->m_startModule = other.m_startModule;
    this->m_appId = other.m_appId;
    this->m_descriptor = other.m_descriptor;
    this->m_version = other.m_version;
    this->m_description = other.m_description;
    this->m_features = other.m_features;
    this->m_changelog = other.m_changelog;
    this->m_default = other.m_default;
    this->m_path = other.m_path;

    return *this;
}

bool AppStartupModuleInformation::operator==(const AppStartupModuleInformation &other) const
{
    return isValid() ? (other.m_startModule == this->m_startModule
                        && other.m_appId == this->m_appId
                        && other.m_descriptor == this->m_descriptor
                        && other.m_version == this->m_version)
                     : true;
}

bool AppStartupModuleInformation::operator!=(const AppStartupModuleInformation &other) const
{
    return !(this->operator==(other));
}

AppStartupModuleInformation &AppStartupModuleInformation::operator=(AppStartupModuleInformation &&other)
{
    this->m_startModule = std::move(other.m_startModule);
    this->m_appId = std::move(other.m_appId);
    this->m_descriptor = std::move(other.m_descriptor);
    this->m_version = std::move(other.m_version);
    this->m_description = std::move(other.m_description);
    this->m_features = std::move(other.m_features);
    this->m_changelog = std::move(other.m_changelog);
    this->m_default = std::move(other.m_default);
    this->m_path = std::move(other.m_path);

    return *this;
}

bool AppStartupModuleInformation::isValid() const
{
    return m_startModule != AppStartupModuleInformation::UnknowModule;
}

AppStartupModuleInformation::StartModule AppStartupModuleInformation::startModule() const
{
    return m_startModule;
}

void AppStartupModuleInformation::setStartModule(StartModule module)
{
    if (module == m_startModule)
        return;

    m_startModule = module;
}

QString AppStartupModuleInformation::appId() const
{
    return m_appId;
}

void AppStartupModuleInformation::setAppId(const QString &appId)
{
    if (appId == m_appId)
        return;

    m_appId = appId;
}

QString AppStartupModuleInformation::descriptor() const
{
    return m_descriptor;
}

void AppStartupModuleInformation::setDescriptor(const QString &descriptor)
{
    if (descriptor == m_descriptor)
        return;

    m_descriptor = descriptor;
}

QString AppStartupModuleInformation::version() const
{
    return m_version;
}

void AppStartupModuleInformation::setVersion(const QString &version)
{
    if (version == m_version)
        return;

    m_version = version;
}

QString AppStartupModuleInformation::description() const
{
    return m_description;
}

void AppStartupModuleInformation::setDescription(const QString &description)
{
    if (description == m_description)
        return;

    m_description = description;
}

QStringList AppStartupModuleInformation::features() const
{
    return m_features;
}

void AppStartupModuleInformation::setFeatures(const QStringList &features)
{
    if (features == m_features)
        return;

    m_features = features;
}

QString AppStartupModuleInformation::changelog() const
{
    return m_changelog;
}

void AppStartupModuleInformation::setChangelog(const QString &changelog)
{
    if (changelog == m_changelog)
        return;

    m_changelog = changelog;
}

bool AppStartupModuleInformation::isDefault() const
{
    return m_default;
}

void AppStartupModuleInformation::setDefault(bool isDefault)
{
    if (isDefault == m_default)
        return;

    m_default = isDefault;
}

QString AppStartupModuleInformation::path() const
{
    return m_path;
}

void AppStartupModuleInformation::setPath(const QString &path)
{
    if (path == m_path)
        return;

    m_path = path;
}

AppStartupModuleGroup::AppStartupModuleGroup()
{

}

AppStartupModuleGroup::AppStartupModuleGroup(std::pair<AppStartupModuleInformation, AppStartupModuleInformation> args)
    : m_group(args)
{

}

AppStartupModuleGroup::AppStartupModuleGroup(const AppStartupModuleGroup &other)
{
    this->m_group = other.m_group;
    this->m_bindingProperties = other.m_bindingProperties;
}

AppStartupModuleGroup::AppStartupModuleGroup(AppStartupModuleGroup &&other)
{
    this->m_group = std::move(other.m_group);
    this->m_bindingProperties = std::move(other.m_bindingProperties);
}

AppStartupModuleGroup &AppStartupModuleGroup::operator=(const AppStartupModuleGroup &other)
{
    this->m_group = other.m_group;
    this->m_bindingProperties = other.m_bindingProperties;
    return *this;
}

AppStartupModuleGroup &AppStartupModuleGroup::operator=(AppStartupModuleGroup &&other)
{
    this->m_group = other.m_group;
    this->m_bindingProperties = other.m_bindingProperties;
    return *this;
}

bool AppStartupModuleGroup::operator==(const AppStartupModuleGroup &other) const
{
    return this->m_group == other.m_group;
}

bool AppStartupModuleGroup::operator!=(const AppStartupModuleGroup &other) const
{
    return this->m_group != other.m_group;
}

bool AppStartupModuleGroup::isValid() const
{
    return m_group.first.isValid() && m_group.second.isValid();
}

AppStartupModuleInformation AppStartupModuleGroup::preload() const
{
    return m_group.first;
}

void AppStartupModuleGroup::setPreload(const AppStartupModuleInformation &preload)
{
    m_group.first = preload;
}

AppStartupModuleInformation AppStartupModuleGroup::entity() const
{
    return m_group.second;
}

void AppStartupModuleGroup::setEntity(const AppStartupModuleInformation &entity)
{
    m_group.second = entity;
}

bool AppStartupModuleGroup::loaded() const
{
    return AppStartupInstance::instance()->loadedModules().contains(*this);
}

void AppStartupModuleGroup::setItemSurface(QQuickItem *item)
{
    m_bindingProperties.insert(BindingProperty::ItemSurface, QVariant::fromValue<QQuickItem *>(item));
}

QVariant AppStartupModuleGroup::bindingProperty(BindingProperty property) const
{
    return m_bindingProperties.value(property);
}
