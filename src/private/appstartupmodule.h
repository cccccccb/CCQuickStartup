#ifndef APPSTARTUPMODULE_H
#define APPSTARTUPMODULE_H

#include <QObject>

#include <private/qquickitemchangelistener_p.h>

#include "appstartupinstance.h"
#include "items/appstartuptransitiongroup.h"
#include "items/appstartupmoduleinformation.h"

class AppStartupInitialProperties;
class AppStartUpTransitionManager;

class AppStartupModule : public QObject, public QQuickItemChangeListener
{
    friend class AppQmlComponentIncubator;
    friend class AppStartupInstancePrivate;
public:
    AppStartupModule(const AppStartupModuleInformation &module, AppStartupInstancePrivate *dd);
    ~AppStartupModule();

    void setBinder(AppStartupModule *binder);
    AppStartupModule *binder() const;

    QPointer<QQuickItem> contentItem() const;
    void setContentItem(QQuickItem *item);

    AppStartupModuleInformation information() const;

    AppStartupModuleGroup group() const;
    void setGroup(const AppStartupModuleGroup &group);

    virtual AppStartupModuleInformation::StartModule moduleType() = 0;
    virtual bool load() { return false; }

    virtual QQuickItem *transitionItem() { return nullptr; }
    virtual QQuickTransition *transition() { return nullptr; }
    virtual AppStartupModule *transitionLinkNext() {return nullptr;}
    virtual AppStartupModule *transitionLinkPrev() {return nullptr;}

    enum TrasitionBeginMode { BeginHead, BeginCurrent };
    virtual bool startTransition(TrasitionBeginMode mode = TrasitionBeginMode::BeginHead);
    virtual void beforeTransition() {}
    virtual void transitionFinish() {}

protected:
    QQmlContext *creationContext(QQmlComponent *component, QObject *obj);

    void initRootItem(QQuickItem *item);
    void deinitRootInit(QQuickItem *item);

    void copyTransitionGroupFromBinder();
    QQmlContext *transitionGroupContextFromBinder() const;

    QQuickWindow *appWindowFromBinder() const;
    QQuickItem *containerContentItemFromBinder();

    QVariantHash initialItemProperties(QObject *target, AppStartupInitialProperties *initialProperties);
    QVariantHash initialItemProperties(QObject *obj, const QVariantHash &properties);

    QObject *loadModule(const QString &path);
    bool unloadModule();

    AppStartupInstance *qq = nullptr;
    AppStartupInstancePrivate *dd = nullptr;

    QPluginLoader *_loader = nullptr;
    AppStartupModule *_binder = nullptr;

    AppStartupModuleInformation _information;
    AppStartupModuleGroup _group;

    bool _appSurfaceIsWindow = false;
    struct SurfacePointer {
        QPointer<QQuickWindow> appSurfaceWindow = nullptr;
        QPointer<QQuickItem> appSurfaceItem = nullptr;
    } _surfacePointer;

    QPointer<QQuickItem> _containerContentItem;


    QPointer<QQuickItem> _contentItem = nullptr;

    bool _duringTransition = false;
    AppStartUpTransitionManager *_transitionManager = nullptr;
    QHash<QQmlComponent *, QQmlContext *> _itemContextMap;
    QPointer<AppStartupTransitionGroup> _transitionGroup = nullptr;

private:
    void transitionFinishedImpl();
};

#endif // APPSTARTUPMODULE_H
