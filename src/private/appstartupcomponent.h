#ifndef APPSTARTUPCOMPONENT_H
#define APPSTARTUPCOMPONENT_H

#include <QObject>

#include <private/qquickitemchangelistener_p.h>

#include "appstartupinstance.h"
#include "items/appstartupcomponentinformation.h"

class AppStartUpTransitionManager;

class AppStartupComponent : public QObject, public QQuickItemChangeListener
{
    friend class AppQmlComponentIncubator;
public:
    AppStartupComponent(const AppStartupComponentInformation &plugin, AppStartupInstancePrivate *dd);
    ~AppStartupComponent();

    void setBinder(AppStartupComponent *binder);
    AppStartupComponent *binder() const;

    virtual AppStartupComponentInformation::StartComponent componentType() = 0;
    virtual void load() {}

    virtual QQuickItem *transitionItem() { return nullptr; }
    virtual QQuickTransition *transition() { return nullptr; }
    virtual AppStartupComponent *transitionLinkNext() {return nullptr;}
    virtual AppStartupComponent *transitionLinkPrev() {return nullptr;}

    enum TrasitionBeginMode { BeginHead, BeginCurrent };
    virtual bool startTransition(TrasitionBeginMode mode = TrasitionBeginMode::BeginHead);
    virtual void transitionFinish() {}

protected:
    QQmlContext *creationContext(QQmlComponent *component, QObject *obj);
    void initRootItem(QQuickItem *item);
    void deinitRootInit(QQuickItem *item);

    AppStartupInstance *qq = nullptr;
    AppStartupInstancePrivate *dd = nullptr;

    AppStartupComponent *_binder = nullptr;
    AppStartupComponentInformation information;

    bool duringTransition = false;
    AppStartUpTransitionManager *transitionManager = nullptr;
    QHash<QQmlComponent *, QQmlContext *> itemContextMap;

private:
    void transitionFinishedImpl();
};

#endif // APPSTARTUPCOMPONENT_H