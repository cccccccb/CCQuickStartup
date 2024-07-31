#include "appstartupinstance_p.h"

#include "appstartupmainwindowcomponent.h"
#include "appstartuppreloadcomponent.h"
#include "ccstartuppreloadinterface.h"
#include "ccstartupmainwindowinterface.h"

#include <QDir>
#include <QLibrary>
#include <QJsonArray>
#include <QPluginLoader>
#include <QQmlApplicationEngine>
#include <QQuickWindow>

class AppStartupComponentFactory
{
public:
    AppStartupComponentFactory(AppStartupInstancePrivate *dd)
        : dd(dd)
    {

    }

    inline AppStartupComponent *create(const AppStartupComponentInformation &plugin)
    {
        if (plugin.startComponent() == AppStartupComponentInformation::Preload) {
            return new AppStartupPreloadComponent(plugin, dd);
        } else if (plugin.startComponent() == AppStartupComponentInformation::MainWindow) {
            return new AppStartupMainWindowComponent(plugin, dd);
        }

        return nullptr;
    }

private:
    AppStartupInstancePrivate *dd;
};

AppStartupInstancePrivate::AppStartupInstancePrivate(AppStartupInstance *qq)
    : qq(qq)
    , engine (nullptr)
    , componentFactory(new AppStartupComponentFactory(this))
    , appWindow(nullptr)
{
    AppStartupInstance::self = qq;
}

AppStartupInstancePrivate::~AppStartupInstancePrivate()
{

}

void AppStartupInstancePrivate::scanPlugins()
{
    QList<AppStartupComponentInformation> plugins;

    // scan the dynamic plugins
    plugins += scanDynamicPlugins();
    // scan the static plugins
    plugins += scanStaticPlugins();

    std::sort(plugins.begin(), plugins.end(),
              [](const AppStartupComponentInformation &before, const AppStartupComponentInformation &after) {
                  return before.startComponent() < after.startComponent()
                         || before.descriptor() < after.descriptor()
                         || before.version() < after.version();
              });

    detachAvailablePluginsChange(plugins);
}

QList<AppStartupComponentInformation> AppStartupInstancePrivate::scanDynamicPlugins()
{
    QList<AppStartupComponentInformation> plugins;

    for (auto iter = pluginPaths.rbegin(); iter != pluginPaths.rend(); ++iter) {
        const QDir dir(*iter);
        if (!dir.exists())
            continue;

        for (const auto &entry : dir.entryInfoList(QDir::Files | QDir::NoSymLinks)) {
            const auto path = entry.absoluteFilePath();
            if (!QLibrary::isLibrary(path))
                continue;

            QPluginLoader loader(path);
            const QJsonObject &metaData = loader.metaData();

            AppStartupComponentInformation info;
            if (!resolveMetaInfoFromObject(metaData, &info))
                continue;

            info.setPath(path);
            qInfo("Find the available plugin %s:%s from the plugin path: %s", qPrintable(info.descriptor()),
                  qPrintable(info.version()), qPrintable(path));

            plugins << info;
        }
    }

    return plugins;
}

QList<AppStartupComponentInformation> AppStartupInstancePrivate::scanStaticPlugins()
{
    QList<AppStartupComponentInformation> plugins;

    auto static_plugins = QPluginLoader::staticPlugins();
    for (auto splg : std::as_const(static_plugins)) {

        const auto &metaData = splg.metaData();
        AppStartupComponentInformation info;
        if (!resolveMetaInfoFromObject(metaData, &info))
            continue;

        qInfo("Find the available plugin %s:%s from the static plugins", qPrintable(info.descriptor()),
              qPrintable(info.version()));

        plugins << info;
    }

    return plugins;
}

void AppStartupInstancePrivate::detachAvailablePluginsChange(const QList<AppStartupComponentInformation> &plugins)
{
    QList<AppStartupComponentGroup> groupList;
    bool availableChanged = false;

    int left = 0;
    int rightLimit = -1;
    while (left < plugins.size() - 1) {
        if ((left > 0 && (plugins[left].startComponent() != plugins[left - 1].startComponent())))
            break;

        if (plugins[left].startComponent() == plugins[left + 1].startComponent()
            && plugins[left].descriptor() == plugins[left + 1].descriptor()) {
            left++;
            continue;
        }

        int right = plugins.size() - 1;
        while ((rightLimit >= 0 ? (right < rightLimit)
                                : (plugins[right].startComponent() != plugins[left].startComponent()))
               && plugins[right].descriptor() != plugins[left].descriptor()) {
            right--;
        }

        const QString &desciptor = plugins[right].descriptor();
        if (desciptor == plugins[left].descriptor()) {
            AppStartupComponentGroup group({plugins[left], plugins[right]});
            if (!availableChanged && !availablePlugins.contains(group)) {
                availableChanged = true;
            }

            groupList.append(group);
            rightLimit = right;
        }

        left++;
    }

    if (availableChanged || groupList.size() != availablePlugins.size()) {
        availablePlugins = groupList;
        Q_EMIT qq->availablePluginsChanged();
    }
}

void AppStartupInstancePrivate::reloadPlugins()
{
    if (reloadPluginsList.isEmpty())
        return;

    unloadPlugins();
    findDefaultComponentGroup();
    loadPreloadPlugins();
}

void AppStartupInstancePrivate::unloadPlugins()
{
    if (appWindow) {
        appWindow->close();
        appWindow->destroy();
        appWindow = nullptr;
    }

    qmlClearTypeRegistrations();
    defaultAppStartItem.clear();
    defaultComponentGroup = {};

    if (engine) {
        engine->clearSingletons();
        engine->clearComponentCache();
    }

    // unload the exist plugins
    auto it = componentPluginHash.begin();
    while (it != componentPluginHash.end()) {
        QString pluginPath = it.key().path();
        QString appId = it.key().appId();

        it = componentPluginHash.erase(it);

        if (!pluginPath.isEmpty()) {
            QPluginLoader loader(pluginPath);
            if (loader.isLoaded()) {
                qInfo() << "Unload the plugin: " << appId << ", from path: "
                        << pluginPath;
                loader.unload();
            }
        }
    }

    windowContentItem.clear();
    engine.reset(new QQmlApplicationEngine(qq));
}

void AppStartupInstancePrivate::findDefaultComponentGroup()
{
    for (const auto &group : reloadPluginsList) {
        if (!group.isValid() || !group.preload().isDefault() || !group.main().isDefault())
            continue;

        this->defaultComponentGroup = group;
        return;
    }
}

void AppStartupInstancePrivate::loadMainWindowPlugins()
{
    while (!reloadPluginsList.isEmpty()) {
        const auto &group = reloadPluginsList.takeFirst();
        AppStartupComponent *component = componentFactory->create(group.main());
        if (!component)
            continue;

        auto preloadComponent = this->componentPluginHash.value(group.preload());
        if (preloadComponent) {
            component->setBinder(preloadComponent.get());
            preloadComponent->setBinder(component);
        }

        this->componentPluginHash.insert(group.main(), QSharedPointer<AppStartupComponent>(component));
        component->load();
    }
}

void AppStartupInstancePrivate::loadPreloadPlugins()
{
    if (!defaultComponentGroup.isValid()) {
        //! @todo add error
        qFatal("No preload plugin found when load the exist plugins!");
        return;
    }

    AppStartupComponent *component = componentFactory->create(defaultComponentGroup.preload());
    if (!component) {
        //! @todo add error
        qFatal("Create preload component failed!");
        return;
    }

    this->componentPluginHash.insert(defaultComponentGroup.preload(), QSharedPointer<AppStartupComponent>(component));
    component->load();
}

bool AppStartupInstancePrivate::resolveMetaInfoFromObject(const QJsonObject &obj, AppStartupComponentInformation *info)
{
    if (!info)
        return false;

    auto metaDataValue = obj.value("MetaData");
    if (!metaDataValue.isObject())
        return false;

    const QString iid = obj["IID"].toString();
    if (iid != qobject_interface_iid<CCStartupPreloadInterface *>()
        && iid != qobject_interface_iid<CCStartupMainWindowInterface *>())
        return false;

    auto metaDataObject = metaDataValue.toObject();
    auto pluginAppId = metaDataObject.value("appid").toString();
    if (pluginAppId != this->appId)
        return false;

    info->setStartComponent(iid == qobject_interface_iid<CCStartupPreloadInterface *>()
                                ? AppStartupComponentInformation::Preload
                                : AppStartupComponentInformation::MainWindow);
    info->setAppId(pluginAppId);
    info->setDescriptor(metaDataObject.value("descriptor").toString());
    info->setVersion(metaDataObject.value("version").toString());
    info->setDescription(metaDataObject.value("description").toString());
    QStringList featuresList;
    const auto &featuresArray = metaDataObject.value("features").toArray();
    for (auto featureValue : featuresArray) {
        featuresList << featureValue.toString();
    }
    info->setFeatures(featuresList);
    info->setChangelog(metaDataObject.value("changelog").toString());
    info->setDefault(metaDataObject.value("default").toBool());

    return true;
}

QStringList AppStartupInstancePrivate::buildinPluginPaths()
{
    QStringList result;
    const auto ccPluginPath = qgetenv("CC_QML_PLUGIN_PATH");
    if (!ccPluginPath.isEmpty())
        result.append(ccPluginPath);

#ifdef CC_QML_APP_PLUGIN_PATH
    result.append(CC_QML_PLUGIN_PATH);
#endif

    return result;
}
