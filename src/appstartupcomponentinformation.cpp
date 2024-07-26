#include "appstartupcomponentinformation.h"

AppStartupComponentInformation::AppStartupComponentInformation()
    : m_startComponent(UnknowComponent)
    , m_default(false)
{

}

AppStartupComponentInformation::AppStartupComponentInformation(const AppStartupComponentInformation &other)
{
    this->m_startComponent = other.m_startComponent;
    this->m_appId = other.m_appId;
    this->m_descriptor = other.m_descriptor;
    this->m_version = other.m_version;
    this->m_description = other.m_description;
    this->m_features = other.m_features;
    this->m_changelog = other.m_changelog;
    this->m_default = other.m_default;
    this->m_path = other.m_path;
}

AppStartupComponentInformation::AppStartupComponentInformation(AppStartupComponentInformation &&other)
{
    this->m_startComponent = std::move(other.m_startComponent);
    this->m_appId = std::move(other.m_appId);
    this->m_descriptor = std::move(other.m_descriptor);
    this->m_version = std::move(other.m_version);
    this->m_description = std::move(other.m_description);
    this->m_features = std::move(other.m_features);
    this->m_changelog = std::move(other.m_changelog);
    this->m_default = std::move(other.m_default);
    this->m_path = std::move(other.m_path);
}

AppStartupComponentInformation &AppStartupComponentInformation::operator=(const AppStartupComponentInformation &other)
{
    this->m_startComponent = other.m_startComponent;
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

bool AppStartupComponentInformation::operator==(const AppStartupComponentInformation &other) const
{
    return isValid() ? (other.m_startComponent == this->m_startComponent
                        && other.m_appId == this->m_appId
                        && other.m_descriptor == this->m_descriptor
                        && other.m_version == this->m_version)
                     : true;
}

bool AppStartupComponentInformation::operator!=(const AppStartupComponentInformation &other) const
{
    return !(this->operator==(other));
}

AppStartupComponentInformation &AppStartupComponentInformation::operator=(AppStartupComponentInformation &&other)
{
    this->m_startComponent = std::move(other.m_startComponent);
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

bool AppStartupComponentInformation::isValid() const
{
    return m_startComponent != AppStartupComponentInformation::UnknowComponent;
}

AppStartupComponentInformation::StartComponent AppStartupComponentInformation::startComponent() const
{
    return m_startComponent;
}

void AppStartupComponentInformation::setStartComponent(StartComponent component)
{
    if (component == m_startComponent)
        return;

    m_startComponent = component;
}

QString AppStartupComponentInformation::appId() const
{
    return m_appId;
}

void AppStartupComponentInformation::setAppId(const QString &appId)
{
    if (appId == m_appId)
        return;

    m_appId = appId;
}

QString AppStartupComponentInformation::descriptor() const
{
    return m_descriptor;
}

void AppStartupComponentInformation::setDescriptor(const QString &descriptor)
{
    if (descriptor == m_descriptor)
        return;

    m_descriptor = descriptor;
}

QString AppStartupComponentInformation::version() const
{
    return m_version;
}

void AppStartupComponentInformation::setVersion(const QString &version)
{
    if (version == m_version)
        return;

    m_version = version;
}

QString AppStartupComponentInformation::description() const
{
    return m_description;
}

void AppStartupComponentInformation::setDescription(const QString &description)
{
    if (description == m_description)
        return;

    m_description = description;
}

QStringList AppStartupComponentInformation::features() const
{
    return m_features;
}

void AppStartupComponentInformation::setFeatures(const QStringList &features)
{
    if (features == m_features)
        return;

    m_features = features;
}

QString AppStartupComponentInformation::changelog() const
{
    return m_changelog;
}

void AppStartupComponentInformation::setChangelog(const QString &changelog)
{
    if (changelog == m_changelog)
        return;

    m_changelog = changelog;
}

bool AppStartupComponentInformation::isDefault() const
{
    return m_default;
}

void AppStartupComponentInformation::setDefault(bool isDefault)
{
    if (isDefault == m_default)
        return;

    m_default = isDefault;
}

QString AppStartupComponentInformation::path() const
{
    return m_path;
}

void AppStartupComponentInformation::setPath(const QString &path)
{
    if (path == m_path)
        return;

    m_path = path;
}

AppStartupComponentGroup::AppStartupComponentGroup()
{

}

AppStartupComponentGroup::AppStartupComponentGroup(std::pair<AppStartupComponentInformation, AppStartupComponentInformation> args)
    : m_group(args)
{

}

AppStartupComponentGroup::AppStartupComponentGroup(const AppStartupComponentGroup &other)
{
    this->m_group = other.m_group;
}

AppStartupComponentGroup::AppStartupComponentGroup(AppStartupComponentGroup &&other)
{
    this->m_group = std::move(other.m_group);
}

AppStartupComponentGroup &AppStartupComponentGroup::operator=(const AppStartupComponentGroup &other)
{
    this->m_group = other.m_group;
    return *this;
}

AppStartupComponentGroup &AppStartupComponentGroup::operator=(AppStartupComponentGroup &&other)
{
    this->m_group = other.m_group;
    return *this;
}

bool AppStartupComponentGroup::operator==(const AppStartupComponentGroup &other) const
{
    return this->m_group == other.m_group;
}

bool AppStartupComponentGroup::operator!=(const AppStartupComponentGroup &other) const
{
    return this->m_group != other.m_group;
}

bool AppStartupComponentGroup::isValid() const
{
    return m_group.first.isValid() && m_group.second.isValid();
}

AppStartupComponentInformation AppStartupComponentGroup::preload() const
{
    return m_group.first;
}

void AppStartupComponentGroup::setPreload(const AppStartupComponentInformation &preload)
{
    m_group.first = preload;
}

AppStartupComponentInformation AppStartupComponentGroup::main() const
{
    return m_group.second;
}

void AppStartupComponentGroup::setMain(const AppStartupComponentInformation &main)
{
    m_group.second = main;
}
