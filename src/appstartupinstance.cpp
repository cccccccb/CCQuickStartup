#include "appstartupinstance.h"
#include "private/appstartupinstance_p.h"
#include "items/appstartupmodulegroup.h"

#include <QEvent>
#include <QGuiApplication>
#include <QQmlApplicationEngine>

AppStartupInstance *AppStartupInstance::self = nullptr;

AppStartupInstance::AppStartupInstance(const QString &appId, const QString &appPath, QObject *parent)
    : QObject (parent)
    , dd(new AppStartupInstancePrivate(this))
{
    dd->appId = appId;

    const auto &paths = dd->buildinModulePaths();
    for (auto iter = paths.rbegin(); iter != paths.rend(); iter++)
        addModulePath(*iter);
    addModulePath(appPath);
}

AppStartupInstance::~AppStartupInstance()
{
    dd->componentModuleHash.clear();
    self = nullptr;
}

QString AppStartupInstance::appId() const
{
    return dd->appId;
}

void AppStartupInstance::addModulePath(const QString &dir)
{
    if (dir.isEmpty())
        return;
    dd->modulePaths.append(dir);
}

QStringList AppStartupInstance::modulePaths() const
{
    QStringList paths;
    std::reverse_copy(dd->modulePaths.begin(), dd->modulePaths.end(), std::back_inserter(paths));
    return paths;
}

void AppStartupInstance::setApplicationFactory(AppStartupApplicationFactory *factory)
{
    dd->applicationFactory = factory;
}

AppStartupApplicationFactory *AppStartupInstance::applicationFactory() const
{
    return dd->applicationFactory;
}

void AppStartupInstance::scanModules()
{
    dd->scanModules();
}

QList<QSharedPointer<AppStartupModuleGroup>> AppStartupInstance::availableModules() const
{
    return dd->availableModules;
}

QSharedPointer<AppStartupModuleGroup> AppStartupInstance::defaultModule() const
{
    return dd->defaultModuleGroup;
}

QList<QSharedPointer<AppStartupModuleGroup>> AppStartupInstance::loadedModules() const
{
    return dd->loadedModulesList;
}

void AppStartupInstance::addReloadModule(const QSharedPointer<AppStartupModuleGroup> &module)
{
    if (dd->reloadModulesList.contains(module))
        return;

    dd->reloadModulesList.append(module);
}

void AppStartupInstance::removeReloadModule(const QSharedPointer<AppStartupModuleGroup> &module)
{
    if (!dd->reloadModulesList.contains(module))
        return;

    dd->reloadModulesList.removeOne(module);
}

void AppStartupInstance::reload()
{
    dd->reloadAllModules();
}

void AppStartupInstance::load(const QSharedPointer<AppStartupModuleGroup> &module)
{
    if (!module->isValid())
        return;

    dd->loadPreloadModules(module);
}

void AppStartupInstance::unload(const QSharedPointer<AppStartupModuleGroup> &module)
{
    if (!module->isValid())
        return;

    dd->unloadModule(module);
}

int AppStartupInstance::exec(int &argc, char **argv)
{
    if (!dd->app)
        dd->createApplication(argc, argv);

    if (!dd->engine)
        dd->createEngine();

    addModulePath(dd->app->applicationDirPath() + QLatin1String("/modules"));

    dd->scanModules();
    dd->reloadModulesList = dd->availableModules;
    if (dd->reloadModulesList.empty()) {
        qFatal("No available modules found!");
        return -1;
    }

    if (!dd->reloadAllModules())
        return -1;

    return dd->app->exec();
}
