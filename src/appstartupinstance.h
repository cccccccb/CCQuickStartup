#ifndef APPSTARTUPINSTANCE_H
#define APPSTARTUPINSTANCE_H

#include <QObject>
#include <QQmlComponent>

#include "ccquickstartup_global.h"

QT_BEGIN_NAMESPACE
class QString;
class QQmlEngine;
class QQuickWindow;
class QGuiApplication;
class QQuickTransition;
QT_END_NAMESPACE

class AppStartupModuleGroup;
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

    QString appId() const;

    void addModulePath(const QString &dir);
    QStringList modulePaths() const;

    void setApplicationFactory(AppStartupApplicationFactory *factory);
    AppStartupApplicationFactory *applicationFactory() const;

    QList<QSharedPointer<AppStartupModuleGroup>> availableModules() const;
    QList<QSharedPointer<AppStartupModuleGroup>> loadedModules() const;
    QSharedPointer<AppStartupModuleGroup> defaultModule() const;

    void addReloadModule(const QSharedPointer<AppStartupModuleGroup> &module);
    void removeReloadModule(const QSharedPointer<AppStartupModuleGroup> &module);

    void reload();
    void scanModules();

    void load(const QSharedPointer<AppStartupModuleGroup> &module);
    void unload(const QSharedPointer<AppStartupModuleGroup> &module);

    int exec(int &argc, char **argv);
    static AppStartupInstance *instance() { return self; }

Q_SIGNALS:
    void loaded(const QSharedPointer<AppStartupModuleGroup> &module);
    void unloaded(const QSharedPointer<AppStartupModuleGroup> &module);
    void availableModulesChanged();

private:
    static AppStartupInstance *self;
    QScopedPointer<AppStartupInstancePrivate> dd;
};

#endif // APPSTARTUPINSTANCE_H
