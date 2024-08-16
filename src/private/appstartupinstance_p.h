#ifndef APPSTARTUPINSTANCE_P_H
#define APPSTARTUPINSTANCE_P_H

#include "appstartupinstance.h"

class QQmlApplicationEngine;
class QQuickItem;

class AppStartupItem;
class AppStartupComponent;
class AppStartupComponentFactory;

class AppStartupInstancePrivate
{
    friend class AppQmlComponentIncubator;
    friend class AppStartupComponent;
public:
    AppStartupInstancePrivate(AppStartupInstance *qq);
    ~AppStartupInstancePrivate();

    void scanPlugins();
    QList<AppStartupComponentInformation> scanDynamicPlugins();
    QList<AppStartupComponentInformation> scanStaticPlugins();
    void detachAvailablePluginsChange(const QList<AppStartupComponentInformation> &plugins);

    bool reloadAllPlugins();
    void unloadAllPlugins();
    void unloadPlugin(const AppStartupComponentGroup &plugin);

    void findDefaultComponentGroup();
    bool loadPreloadPlugins(const AppStartupComponentGroup &plugin);
    void loadEntityPlugins(const AppStartupComponentGroup &plugin);
    bool resolveMetaInfoFromObject(const QJsonObject &obj, AppStartupComponentInformation *info);
    void processCommandLineArguments();

    void createApplication(int &argc, char **argv);
    void createEngine();

    static QStringList buildinPluginPaths();

    AppStartupInstance *qq = nullptr;
    QScopedPointer<QGuiApplication> app;
    QScopedPointer<QQmlApplicationEngine> engine;

    QString appId;

    AppStartupApplicationFactory *applicationFactory = nullptr;
    QScopedPointer<AppStartupComponentFactory> componentFactory;
    AppStartupComponentGroup defaultComponentGroup;

    QStringList pluginPaths;

    QList<AppStartupComponentGroup> availablePlugins;
    QList<AppStartupComponentGroup> reloadPluginsList;
    QList<AppStartupComponentGroup> loadedPluginsList;

    QHash<AppStartupComponentInformation, QSharedPointer<AppStartupComponent>> componentPluginHash;
};

#endif // APPSTARTUPINSTANCE_P_H
