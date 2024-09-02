#ifndef APPSTARTUPINSTANCE_P_H
#define APPSTARTUPINSTANCE_P_H

#include "appstartupinstance.h"
#include "items/appstartupmoduleinformation.h"

class QQmlApplicationEngine;
class QQuickItem;

class AppStartupItem;
class AppStartupModuleObject;
class AppStartupModuleFactory;

class AppStartupInstancePrivate
{
    friend class AppQmlComponentIncubator;
    friend class AppStartupModule;
public:
    AppStartupInstancePrivate(AppStartupInstance *qq);
    ~AppStartupInstancePrivate();

    void scanModules();
    QList<AppStartupModuleInformation> scanDynamicModules();
    QList<AppStartupModuleInformation> scanStaticModules();
    void detachAvailableModulesChange(const QList<AppStartupModuleInformation> &modules);

    bool reloadAllModules();
    void unloadAllModules();
    void unloadModule(const QSharedPointer<AppStartupModuleGroup> &module);

    void findDefaultModuleGroup();
    bool loadPreloadModules(const QSharedPointer<AppStartupModuleGroup> &module);
    void loadEntityModules(const QSharedPointer<AppStartupModuleGroup> &module);
    void processCommandLineArguments();

    void createApplication(int &argc, char **argv);
    void createEngine();

    static QStringList buildinModulePaths();
    static bool resolveInformation(const QJsonObject &obj, AppStartupModuleInformation *info);
    static bool resolveInformation(const QString &path, AppStartupModuleInformation *out);

    AppStartupInstance *qq = nullptr;
    QScopedPointer<QGuiApplication> app;
    QScopedPointer<QQmlApplicationEngine> engine;

    QString appId;

    AppStartupApplicationFactory *applicationFactory = nullptr;
    QScopedPointer<AppStartupModuleFactory> moduleFactory;
    QSharedPointer<AppStartupModuleGroup> defaultModuleGroup;

    QStringList modulePaths;

    QList<QSharedPointer<AppStartupModuleGroup>> availableModules;
    QList<QSharedPointer<AppStartupModuleGroup>> reloadModulesList;
    QList<QSharedPointer<AppStartupModuleGroup>> loadedModulesList;

    QHash<AppStartupModuleInformation, QSharedPointer<AppStartupModuleObject>> componentModuleHash;
};

#endif // APPSTARTUPINSTANCE_P_H
