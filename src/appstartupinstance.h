#ifndef APPSTARTUPINSTANCE_H
#define APPSTARTUPINSTANCE_H

#include <QObject>
#include <QQmlComponent>

#include "ccquickstartup_global.h"
#include "items/appstartupcomponentinformation.h"

QT_BEGIN_NAMESPACE
class QString;
class QQmlEngine;
class QQuickWindow;
class QGuiApplication;
class QQuickTransition;
QT_END_NAMESPACE

class AppStartupApplicationFactory;
class AppStartupInstancePrivate;

class CC_QUICKSTARTUP_EXPORT AppStartupInstance : public QObject
{
    Q_OBJECT
    friend class AppStartupInstancePrivate;

public:
    AppStartupInstance() = delete;
    AppStartupInstance(const QString &appId, const QString &appPath = QString(), QObject *parent = nullptr);
    ~AppStartupInstance();

    void addPluginPath(const QString &dir);
    QStringList pluginPaths() const;

    void setApplicationFactory(AppStartupApplicationFactory *factory);
    AppStartupApplicationFactory *applicationFactory() const;

    QList<AppStartupComponentGroup> availablePlugins() const;
    QList<AppStartupComponentGroup> loadedPlugins() const;
    AppStartupComponentGroup defaultPlugin() const;

    void addReloadPlugin(const AppStartupComponentGroup &plugin);
    void removeReloadPlugin(const AppStartupComponentGroup &plugin);

    void reload();
    void scanPlugins();

    void load(const AppStartupComponentGroup &plugin);
    void unload(const AppStartupComponentGroup &plugin);

    int exec(int &argc, char **argv);
    static AppStartupInstance *instance() { return self; }

Q_SIGNALS:
    void loadFinished(const AppStartupComponentGroup &plugin);
    void availablePluginsChanged();

private:
    static AppStartupInstance *self;
    QScopedPointer<AppStartupInstancePrivate> dd;
};

#endif // APPSTARTUPINSTANCE_H
