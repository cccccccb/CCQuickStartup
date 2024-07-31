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

    void reloadPlugins();
    void unloadPlugins();

    void findDefaultComponentGroup();
    void loadPreloadPlugins();
    void loadMainWindowPlugins();
    bool resolveMetaInfoFromObject(const QJsonObject &obj, AppStartupComponentInformation *info);

    static QStringList buildinPluginPaths();

    AppStartupInstance *qq = nullptr;
    QScopedPointer<QGuiApplication> app;
    QScopedPointer<QQmlApplicationEngine> engine;

    QString appId;

    QScopedPointer<AppStartupComponentFactory> componentFactory;
    QQuickWindow *appWindow = nullptr;
    QPointer<QQuickItem> windowContentItem;
    QPointer<AppStartupItem> defaultAppStartItem;
    AppStartupComponentGroup defaultComponentGroup;

    QStringList pluginPaths;

    QList<AppStartupComponentGroup> availablePlugins;
    QList<AppStartupComponentGroup> reloadPluginsList;
    QList<AppStartupComponentGroup> loadedPluginsList;

    QHash<AppStartupComponentInformation, QSharedPointer<AppStartupComponent>> componentPluginHash;
};

#endif // APPSTARTUPINSTANCE_P_H
