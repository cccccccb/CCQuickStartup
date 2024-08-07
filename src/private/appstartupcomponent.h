#ifndef APPSTARTUPCOMPONENT_H
#define APPSTARTUPCOMPONENT_H

#include <QObject>

#include <private/qquickitemchangelistener_p.h>

#include "appstartupinstance.h"
#include "items/appstartuptransitiongroup.h"
#include "items/appstartupcomponentinformation.h"

class AppStartupInitialProperties;
class AppStartUpTransitionManager;

class AppStartupComponent : public QObject, public QQuickItemChangeListener
{
    friend class AppQmlComponentIncubator;
public:
    AppStartupComponent(const AppStartupComponentInformation &plugin, AppStartupInstancePrivate *dd);
    ~AppStartupComponent();

    void setBinder(AppStartupComponent *binder);
    AppStartupComponent *binder() const;

    QPointer<QQuickItem> contentItem() const;
    void setContentItem(QQuickItem *item);

    virtual AppStartupComponentInformation::StartComponent componentType() = 0;
    virtual bool load() { return false; }

    virtual QQuickItem *transitionItem() { return nullptr; }
    virtual QQuickTransition *transition() { return nullptr; }
    virtual AppStartupComponent *transitionLinkNext() {return nullptr;}
    virtual AppStartupComponent *transitionLinkPrev() {return nullptr;}

    enum TrasitionBeginMode { BeginHead, BeginCurrent };
    virtual bool startTransition(TrasitionBeginMode mode = TrasitionBeginMode::BeginHead);
    virtual void beforeTransition() {}
    virtual void transitionFinish() {}

protected:
    QQmlContext *creationContext(QQmlComponent *component, QObject *obj);
    void initRootItem(QQuickItem *item);
    void deinitRootInit(QQuickItem *item);

    void copyTransitionGroupFromBinder();
    QQmlContext *transitionGroupContextFromBinder();
    QVariantHash initialItemProperties(QObject *target, AppStartupInitialProperties *initialProperties);
    QVariantHash initialItemProperties(QObject *obj, const QVariantHash &properties);

    AppStartupInstance *qq = nullptr;
    AppStartupInstancePrivate *dd = nullptr;

    AppStartupComponent *_binder = nullptr;
    AppStartupComponentInformation information;

    QPointer<QQuickItem> _contentItem = nullptr;

    bool duringTransition = false;
    AppStartUpTransitionManager *transitionManager = nullptr;
    QHash<QQmlComponent *, QQmlContext *> itemContextMap;
    QPointer<AppStartupTransitionGroup> transitionGroup = nullptr;

private:
    void transitionFinishedImpl();
};

#endif // APPSTARTUPCOMPONENT_H
