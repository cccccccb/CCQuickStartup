#ifndef APPSTARTUPMODULEOBJECT_H
#define APPSTARTUPMODULEOBJECT_H

#include <QObject>

#include <private/qquickitemchangelistener_p.h>

#include "appstartupinstance.h"
#include "items/appstartupmodulegroup.h"
#include "items/appstartuptransitiongroup.h"
#include "items/appstartupmoduleinformation.h"

class AppStartupInitialProperties;
class AppStartUpTransitionManager;

class AppStartupModuleObject : public QObject, public QQuickItemChangeListener
{
    friend class AppQmlComponentIncubator;
    friend class AppStartupInstancePrivate;
public:
    AppStartupModuleObject(const AppStartupModuleInformation &module, AppStartupInstancePrivate *dd);
    ~AppStartupModuleObject();

    void setBinder(AppStartupModuleObject *binder);
    AppStartupModuleObject *binder() const;

    QPointer<QQuickItem> contentItem() const;
    void setContentItem(QQuickItem *item);

    AppStartupModuleInformation information() const;

    QSharedPointer<AppStartupModuleGroup> group() const;
    void setGroup(const QSharedPointer<AppStartupModuleGroup> &group);
    QVariant moduleBindingProperty(AppStartupModuleGroup::BindingProperty type);
    void insertModuleBindingProperty(AppStartupModuleGroup::BindingProperty type, const QVariant &value);
    void insertModuleBindingPropertyList(AppStartupModuleGroup::BindingProperty type, const QVariantList &value);

    virtual AppStartupModuleInformation::StartModule moduleType() = 0;
    virtual bool load() { return false; }

    virtual QQuickItem *transitionItem() { return nullptr; }
    virtual QQuickTransition *transition() { return nullptr; }
    virtual AppStartupModuleObject *transitionLinkNext() {return nullptr;}
    virtual AppStartupModuleObject *transitionLinkPrev() {return nullptr;}

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
    AppStartupModuleObject *_binder = nullptr;

    AppStartupModuleInformation _information;
    QSharedPointer<AppStartupModuleGroup> _group;

    bool _appSurfaceIsWindow = false;
    struct SurfacePointer {
        QPointer<QQuickWindow> appSurfaceWindow;
        QPointer<QQuickItem> appSurfaceItem;
    } _surfacePointer;

    QPointer<QQuickItem> _containerContentItem;
    QPointer<QQuickItem> _contentItem;

    bool _duringTransition = false;
    AppStartUpTransitionManager *_transitionManager = nullptr;
    QHash<QQmlComponent *, QQmlContext *> _itemContextMap;
    QPointer<AppStartupTransitionGroup> _transitionGroup = nullptr;

private:
    void transitionFinishedImpl();
};

#endif // APPSTARTUPMODULEOBJECT_H
