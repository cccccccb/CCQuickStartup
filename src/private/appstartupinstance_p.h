#ifndef APPSTARTUPINSTANCE_P_H
#define APPSTARTUPINSTANCE_P_H

#include "appstartupinstance.h"

class QQmlApplicationEngine;
class QQuickItem;

class AppStartupItem;
class AppStartupModule;
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
    void unloadModule(const AppStartupModuleGroup &module);

    void findDefaultModuleGroup();
    bool loadPreloadModules(const AppStartupModuleGroup &module);
    void loadEntityModules(const AppStartupModuleGroup &module);
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
    AppStartupModuleGroup defaultModuleGroup;

    QStringList modulePaths;

    QList<AppStartupModuleGroup> availableModules;
    QList<AppStartupModuleGroup> reloadModulesList;
    QList<AppStartupModuleGroup> loadedModulesList;

    QHash<AppStartupModuleInformation, QSharedPointer<AppStartupModule>> componentModuleHash;
};

#endif // APPSTARTUPINSTANCE_P_H
