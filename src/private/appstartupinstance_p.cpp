#include "appstartupinstance_p.h"

#include "appstartupapplicationfactory.h"
#include "appstartupentitymoduleobject.h"
#include "appstartuppreloadmoduleobject.h"

#include "interface/appstartuppreloadinterface.h"
#include "interface/appstartupentityinterface.h"
#include "items/appstartupmodulegroup.h"

#include <QDir>
#include <QLibrary>
#include <QJsonArray>
#include <QPluginLoader>
#include <QQmlApplicationEngine>
#include <QQuickWindow>

class AppStartupModuleFactory
{
public:
    AppStartupModuleFactory(AppStartupInstancePrivate *dd)
        : dd(dd)
    {

    }

    inline AppStartupModuleObject *create(const AppStartupModuleInformation &module)
    {
        if (module.startModule() == AppStartupModuleInformation::Preload) {
            return new AppStartupPreloadModuleObject(module, dd);
        } else if (module.startModule() == AppStartupModuleInformation::Entity) {
            return new AppStartupEntityModuleObject(module, dd);
        }

        return nullptr;
    }

private:
    AppStartupInstancePrivate *dd;
};

AppStartupInstancePrivate::AppStartupInstancePrivate(AppStartupInstance *qq)
    : qq(qq)
    , engine (nullptr)
    , moduleFactory(new AppStartupModuleFactory(this))
{
    AppStartupInstance::self = qq;
}

AppStartupInstancePrivate::~AppStartupInstancePrivate()
{

}

void AppStartupInstancePrivate::scanModules()
{
    QList<AppStartupModuleInformation> modules;

    // scan the dynamic modules
    modules += scanDynamicModules();
    // scan the static modules
    modules += scanStaticModules();

    std::sort(modules.begin(), modules.end(),
              [](const AppStartupModuleInformation &before, const AppStartupModuleInformation &after) {
        return before.startModule() < after.startModule()
               || before.descriptor() < after.descriptor()
               || before.version() < after.version();
    });

    detachAvailableModulesChange(modules);
}

QList<AppStartupModuleInformation> AppStartupInstancePrivate::scanDynamicModules()
{
    QList<AppStartupModuleInformation> modules;

    for (auto iter = modulePaths.rbegin(); iter != modulePaths.rend(); ++iter) {
        const QDir dir(*iter);
        if (!dir.exists())
            continue;

        for (const auto &entry : dir.entryInfoList(QDir::Files | QDir::NoSymLinks)) {
            const auto path = entry.absoluteFilePath();
            AppStartupModuleInformation info;
            if (!resolveInformation(path, &info))
                continue;

            info.setPath(path);
            qInfo("Find the available module %s:%s from the module path: %s", qPrintable(info.descriptor()),
                  qPrintable(info.version()), qPrintable(path));

            modules << info;
        }
    }

    return modules;
}

QList<AppStartupModuleInformation> AppStartupInstancePrivate::scanStaticModules()
{
    QList<AppStartupModuleInformation> modules;

    auto staticModules = QPluginLoader::staticPlugins();
    for (auto splg : std::as_const(staticModules)) {

        const auto &metaData = splg.metaData();
        AppStartupModuleInformation info;
        if (!resolveInformation(metaData, &info))
            continue;

        qInfo("Find the available module %s:%s from the static modules", qPrintable(info.descriptor()),
              qPrintable(info.version()));

        modules << info;
    }

    return modules;
}

void AppStartupInstancePrivate::detachAvailableModulesChange(const QList<AppStartupModuleInformation> &modules)
{
    QList<QSharedPointer<AppStartupModuleGroup>> groupList;
    bool availableChanged = false;

    int left = 0;
    int rightLimit = -1;
    while (left < modules.size() - 1) {
        if ((left > 0 && (modules[left].startModule() != modules[left - 1].startModule())))
            break;

        if (modules[left].startModule() == modules[left + 1].startModule()
            && modules[left].descriptor() == modules[left + 1].descriptor()) {
            left++;
            continue;
        }

        int right = modules.size() - 1;
        while ((rightLimit >= 0 ? (right < rightLimit)
                                : (modules[right].startModule() != modules[left].startModule()))
               && modules[right].descriptor() != modules[left].descriptor()) {
            right--;
        }

        const QString &desciptor = modules[right].descriptor();
        if (desciptor == modules[left].descriptor()) {
            QSharedPointer<AppStartupModuleGroup> group(new AppStartupModuleGroup({modules[left], modules[right]}, qq));
            if (!availableChanged && !availableModules.contains(group)) {
                availableChanged = true;
            }

            groupList.append(group);
            rightLimit = right;
        }

        left++;
    }

    if (availableChanged || groupList.size() != availableModules.size()) {
        availableModules = groupList;
        Q_EMIT qq->availableModulesChanged();
    }
}

bool AppStartupInstancePrivate::reloadAllModules()
{
    reloadModulesList += loadedModulesList;

    unloadAllModules();
    findDefaultModuleGroup();
    if (!this->defaultModuleGroup->isValid())
        return false;

    reloadModulesList.removeOne(defaultModuleGroup);
    return loadPreloadModules(defaultModuleGroup);
}

void AppStartupInstancePrivate::unloadAllModules()
{
    defaultModuleGroup = {};

    if (engine)
        engine->clearComponentCache();

    // unload the exist modules
    auto it = componentModuleHash.begin();
    while (it != componentModuleHash.end()) {
        it = componentModuleHash.erase(it);
    }

    loadedModulesList.clear();
}

void AppStartupInstancePrivate::unloadModule(const QSharedPointer<AppStartupModuleGroup> &module)
{
    // unload the exist modules
    auto it = componentModuleHash.begin();
    while (it != componentModuleHash.end()) {
        if (it.key() == module->preload() || it.key() == module->entity()) {
            it = componentModuleHash.erase(it);
        } else {
            it++;
        }
    }

    loadedModulesList.removeOne(module);
    Q_EMIT qq->unloaded(module);
}

void AppStartupInstancePrivate::findDefaultModuleGroup()
{
    QSharedPointer<AppStartupModuleGroup> moduleGroup;
    for (const auto &group : reloadModulesList) {
        if (!group->isValid() || !group->preload().isDefault() || !group->entity().isDefault())
            continue;

        if (!moduleGroup || moduleGroup->entity().version() < group->entity().version())
            moduleGroup = group;
    }

    if (moduleGroup->isValid())
        this->defaultModuleGroup = moduleGroup;
}

void AppStartupInstancePrivate::loadEntityModules(const QSharedPointer<AppStartupModuleGroup> &module)
{
    AppStartupModuleObject *moduleObject = moduleFactory->create(module->entity());
    if (!moduleObject)
        return;

    auto preloadModule = this->componentModuleHash.value(module->preload());
    if (preloadModule) {
        moduleObject->setBinder(preloadModule.get());
        preloadModule->setBinder(moduleObject);
    }

    moduleObject->setGroup(module);
    this->componentModuleHash.insert(module->entity(), QSharedPointer<AppStartupModuleObject>(moduleObject));
    moduleObject->load();
}

bool AppStartupInstancePrivate::loadPreloadModules(const QSharedPointer<AppStartupModuleGroup> &module)
{
    if (!module->isValid()) {
        //! @todo add error
        qFatal("No preload module found when load the exist modules!");
        return false;
    }

    AppStartupModuleObject *moduleObject = moduleFactory->create(module->preload());
    if (!moduleObject) {
        //! @todo add error
        qFatal("Create preload module failed!");
        return false;
    }

    moduleObject->setGroup(module);
    this->componentModuleHash.insert(module->preload(), QSharedPointer<AppStartupModuleObject>(moduleObject));
    return moduleObject->load();
}

bool AppStartupInstancePrivate::resolveInformation(const QJsonObject &obj, AppStartupModuleInformation *info)
{
    if (!info)
        return false;

    auto metaDataValue = obj.value("MetaData");
    if (!metaDataValue.isObject())
        return false;

    const QString iid = obj["IID"].toString();
    if (iid != qobject_interface_iid<AppStartupPreloadInterface *>()
        && iid != qobject_interface_iid<AppStartupEntityInterface *>())
        return false;

    auto metaDataObject = metaDataValue.toObject();
    auto moduleAppId = metaDataObject.value("appid").toString();
    if (moduleAppId != AppStartupInstance::instance()->appId())
        return false;

    info->setStartModule(iid == qobject_interface_iid<AppStartupPreloadInterface *>()
                                ? AppStartupModuleInformation::Preload
                                : AppStartupModuleInformation::Entity);
    info->setAppId(moduleAppId);
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

void AppStartupInstancePrivate::createApplication(int &argc, char **argv)
{
    if (this->app)
        return;

    if (applicationFactory)
        this->app.reset(applicationFactory->createApplication(argc, argv));

    if (!this->app)
        this->app.reset(new QGuiApplication(argc, argv));
}

void AppStartupInstancePrivate::createEngine()
{
    if (this->engine)
        return;

    if (applicationFactory)
        this->engine.reset(applicationFactory->createEngine());

    if (!this->engine)
        this->engine.reset(new QQmlApplicationEngine(qq));
}

QStringList AppStartupInstancePrivate::buildinModulePaths()
{
    QStringList result;
    const auto ccModulePath = qgetenv("CC_QML_APP_MODULE_PATH");
    if (!ccModulePath.isEmpty())
        result.append(ccModulePath);

#ifdef CC_QML_APP_MODULE_PATH
    result.append(CC_QML_APP_MODULE_PATH);
#endif

    return result;
}

bool AppStartupInstancePrivate::resolveInformation(const QString &path, AppStartupModuleInformation *out)
{
    if (!QLibrary::isLibrary(path))
        return false;

    if (!out)
        return false;

    QPluginLoader loader(path);
    const QJsonObject &metaData = loader.metaData();

    if (!resolveInformation(metaData, out))
        return false;

    return true;
}
