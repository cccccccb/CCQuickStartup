#include "appstartupinstance.h"
#include "private/appstartupinstance_p.h"

#include <QEvent>
#include <QGuiApplication>
#include <QQmlApplicationEngine>

AppStartupInstance *AppStartupInstance::self = nullptr;

AppStartupInstance::AppStartupInstance(const QString &appId, const QString &appPath, QObject *parent)
    : QObject (parent)
    , dd(new AppStartupInstancePrivate(this))
{
    dd->appId = appId;

    const auto &paths = dd->buildinPluginPaths();
    for (auto iter = paths.rbegin(); iter != paths.rend(); iter++)
        addPluginPath(*iter);
    addPluginPath(appPath);
}

AppStartupInstance::~AppStartupInstance()
{
    dd->componentPluginHash.clear();
    self = nullptr;
}

void AppStartupInstance::addPluginPath(const QString &dir)
{
    if (dir.isEmpty())
        return;
    dd->pluginPaths.append(dir);
}

QStringList AppStartupInstance::pluginPaths() const
{
    QStringList paths;
    std::reverse_copy(dd->pluginPaths.begin(), dd->pluginPaths.end(), std::back_inserter(paths));
    return paths;
}

void AppStartupInstance::scanPlugins()
{
    dd->scanPlugins();
}

QList<AppStartupComponentGroup> AppStartupInstance::availablePlugins() const
{
    return dd->availablePlugins;
}

QList<AppStartupComponentGroup> AppStartupInstance::loadedPlugins() const
{
    return dd->loadedPluginsList;
}

void AppStartupInstance::addReloadPlugin(const AppStartupComponentGroup &plugin)
{
    if (dd->reloadPluginsList.contains(plugin))
        return;

    dd->reloadPluginsList.append(plugin);
}

void AppStartupInstance::removeReloadPlugin(const AppStartupComponentGroup &plugin)
{
    if (!dd->reloadPluginsList.contains(plugin))
        return;

    dd->reloadPluginsList.removeOne(plugin);
}

void AppStartupInstance::reload()
{
    dd->reloadPlugins();
}

int AppStartupInstance::exec(int &argc, char **argv)
{
    dd->scanPlugins();
    dd->reloadPluginsList = dd->availablePlugins;
    if (dd->reloadPluginsList.empty()) {
        qFatal("No available plugins found!");
        return -1;
    }

    if (!dd->app)
        dd->app.reset(new QGuiApplication(argc, argv));

    if (!dd->reloadPlugins())
        return -1;

    return dd->app->exec();
}
