#include "appstartupmainwindowcomponent.h"
#include "ccstartupmainwindowinterface.h"
#include "appstartupinstance_p.h"
#include "defines_p.h"

#include "items/appstartupinitialproperties.h"
#include "items/appstartupinstanceattached.h"
#include "items/appstartuppreloadattached.h"
#include "items/appstartuptransitiongroup.h"
#include "items/appstartupitem.h"

#include <QPluginLoader>
#include <QQmlIncubator>
#include <QQmlComponent>
#include <QQmlApplicationEngine>

#include <private/qquicktransition_p.h>

class AppQmlComponentIncubator : public QQmlIncubator
{
public:
    AppQmlComponentIncubator(QQmlComponent *component, AppStartupMainWindowComponent *appExtra,
                             IncubationMode mode = AsynchronousIfNested);

protected:
    virtual void statusChanged(Status) override;
    virtual void setInitialState(QObject *) override;

private:
    AppStartupMainWindowComponent *mainComponent;
    QQmlComponent *compnent;
};

AppQmlComponentIncubator::AppQmlComponentIncubator(QQmlComponent *component, AppStartupMainWindowComponent *mainComponent, QQmlIncubator::IncubationMode mode)
    : QQmlIncubator(mode)
    , mainComponent(mainComponent)
    , compnent(component)
{

}

void AppQmlComponentIncubator::statusChanged(QQmlIncubator::Status status)
{
    if (status != QQmlIncubator::Ready) {
        if (status == QQmlIncubator::Error)
            qWarning() << "AppStartupInstance: " << this->errors();
        return;
    }

    QObject *obj = object();
    QQuickItem *item = qmlobject_cast<QQuickItem *>(obj);
    if (item) {
        item->setParentItem(mainComponent->appRootItem);
    }

    AppStartupInstanceAttached *attached = qobject_cast<AppStartupInstanceAttached*>(
        qmlAttachedPropertiesObject<AppStartupInstanceAttached>(mainComponent->appRootItem, false));

    if (attached) {
        QQmlContext *context = qmlContext(obj);
        attached->insertSubObject(context->nameForObject(obj).toLocal8Bit(), obj);
    }

    mainComponent->_q_onComponentProgressChanged();
    mainComponent->destoryIncubator(this);
}

void AppQmlComponentIncubator::setInitialState(QObject *o)
{
    if (!mainComponent->appRootItem)
        return;

    if (o) {
        QQmlContext *context = mainComponent->itemContextMap.value(compnent);
        if (context)
            QQml_setParent_noEvent(context, o);
        QQml_setParent_noEvent(o, mainComponent->appRootItem);
        if (QQuickItem *item = qmlobject_cast<QQuickItem *>(o))
            item->setParentItem(mainComponent->appRootItem);
    }
}

AppStartupMainWindowComponent::~AppStartupMainWindowComponent()
{
    if (dd->windowContentItem)
        deinitRootInit(dd->windowContentItem);
}

QQuickItem *AppStartupMainWindowComponent::transitionItem()
{
    return appRootItem;
}

QQuickTransition *AppStartupMainWindowComponent::transition()
{
    return transitionGroup ? transitionGroup->enter() : nullptr;
}

AppStartupComponent *AppStartupMainWindowComponent::transitionLinkPrev()
{
    return binder();
}

void AppStartupMainWindowComponent::transitionFinish()
{
    endOfTransition();
}

void AppStartupMainWindowComponent::beforeTransition()
{
    if (transitionGroup)
        initialItemProperties(appRootItem, transitionGroup->enterInitialProperties());
}

void AppStartupMainWindowComponent::load()
{
    const AppStartupComponentInformation &pluginInfo = this->information;
    QPluginLoader loader(pluginInfo.path());
    mainInstance.reset(qobject_cast<CCStartupMainWindowInterface *>(loader.instance()));
    if (!mainInstance) {
        //! @todo add error
        qWarning("load main window plugin failed!");
        return;
    }

    mainInstance->initialize(dd->engine.get());
    // Insert component into preload
    const QUrl &mainComponentPath = mainInstance->mainComponentPath();
    mainComponent = new QQmlComponent(dd->engine.get(), mainComponentPath, QQmlComponent::Asynchronous);
    if (mainComponent->isLoading()) {
        QObject::connect(mainComponent, &QQmlComponent::statusChanged,
                         this, &AppStartupMainWindowComponent::_q_onMainComponentStatusChanged);
    } else {
        _q_onMainComponentStatusChanged(mainComponent->status());
    }
}

void AppStartupMainWindowComponent::finishedLoaded()
{
    mainInstance->finishedLoading(dd->engine.get());
    appRootItem->setLoaded(true);

    AppStartupInstanceAttached *itemAttached = qobject_cast<AppStartupInstanceAttached*>(qmlAttachedPropertiesObject<AppStartupInstanceAttached>(appRootItem, false));
    if (itemAttached)
        itemAttached->setLoaded(true);

    if (appRootItem == dd->defaultAppStartItem) {
        AppStartupPreloadAttached *preloadAttached = qobject_cast<AppStartupPreloadAttached*>(qmlAttachedPropertiesObject<AppStartupPreloadAttached>(dd->appWindow, false));
        if (preloadAttached)
            preloadAttached->setLoaded(true);

        Q_EMIT qq->loadFinished();

        if (!preloadAttached || preloadAttached->autoExitOverlay())
            startTransition();

        appRootItem->setFocus(true);
    }
}

void AppStartupMainWindowComponent::endOfTransition()
{
    if (appRootItem) {
        appRootItem->setEnabled(true);
        appRootItem->setFocus(true);
        appRootItem->setVisible(true);
    }
}

void AppStartupMainWindowComponent::destoryIncubator(QQmlIncubator *incubator)
{
    incubators.removeAll(incubator);
    childrenCount--;

    if (childrenCount == 0)
        finishedLoaded();

    incubator->clear();
    delete incubator;
}

void AppStartupMainWindowComponent::_q_onMainComponentStatusChanged(QQmlComponent::Status status)
{
    if (status != QQmlComponent::Ready) {
        if (status == QQmlComponent::Error)
            qWarning() << "AppStartupInstance: " << mainComponent->errors() << " " << mainComponent->errorString();
        return;
    }

    do {
        // ApplicationWindow.
        if (createObjects(APPLICATIONWINDOW_CONTENTDATA))
            break;

        // DialogWindow.
        if (createObjects(DIALOGWINDOW_CONTENTDATA))
            break;

        // Window.
        if (createObjects(WINDOW_CONTENTDATA))
            break;
    } while (false);

    mainComponent->deleteLater();
    mainComponent = nullptr;
}

void AppStartupMainWindowComponent::_q_onComponentProgressChanged()
{
    qreal progress = 0;
    auto components = appRootItem->findChildren<QQmlComponent *>();
    for (auto childCom : std::as_const(components)) {
        progress += childCom->progress();
    }

    appRootItem->setProgress(progress / components.count());
}

void AppStartupMainWindowComponent::itemGeometryChanged(QQuickItem *item, QQuickGeometryChange change, const QRectF &oldGeometry)
{
    updateRootItemSize(item);
    QQuickItemChangeListener::itemGeometryChanged(item, change, oldGeometry);
}

bool AppStartupMainWindowComponent::createObjects(const char *propertyName)
{
    Q_ASSERT(mainComponent);
    QQmlListReference pros(dd->appWindow, propertyName);
    if (!pros.isValid())
        return false;

    copyTransitionGroupFromBinder();
    appRootItem = qobject_cast<AppStartupItem *>(mainComponent->beginCreate(creationContext(mainComponent, dd->appWindow)));
    Q_ASSERT_X(appRootItem, "AppStartupInstance", "Must use the AppLoader item in main component.");

    if (QQmlContext *context = transitionGroupContextFromBinder())
        context->setContextProperty(QLatin1String("enterTarget"), appRootItem);

    appRootItem->setContainWindow(dd->appWindow);
    appRootItem->setEnabled(false);
    appRootItem->setVisible(false);

    mainComponent->completeCreate();

    if (dd->defaultComponentGroup.main() == this->information)
        dd->defaultAppStartItem = appRootItem;

    AppStartupPreloadAttached *preloadAttached = nullptr;
    if (dd->defaultComponentGroup.main() == this->information) {
        preloadAttached = qobject_cast<AppStartupPreloadAttached*>(qmlAttachedPropertiesObject<AppStartupPreloadAttached>(dd->appWindow, false));
        preloadAttached->setStartupItem(appRootItem);
    }

    pros.append(appRootItem);
    initRootItem(dd->windowContentItem);
    updateRootItemSize(dd->windowContentItem);
    createChildComponents();
    return true;
}

void AppStartupMainWindowComponent::createChildComponents()
{
    auto components = appRootItem->findChildren<QQmlComponent *>(QStringLiteral(""), Qt::FindDirectChildrenOnly);
    childrenCount = components.size();

    if (childrenCount == 0) {
        finishedLoaded();
        return;
    }

    for (auto childCom : std::as_const(components)) {
        QObject::connect(childCom, &QQmlComponent::progressChanged, this,
                         &AppStartupMainWindowComponent::_q_onComponentProgressChanged);
        auto asyn = appRootItem->asynchronous() ? AppQmlComponentIncubator::Asynchronous : AppQmlComponentIncubator::AsynchronousIfNested;
        AppQmlComponentIncubator *incubator = new AppQmlComponentIncubator(childCom, this, asyn);
        this->incubators.append(incubator);
        childCom->create(*incubator, creationContext(childCom, appRootItem));
    }
}

void AppStartupMainWindowComponent::updateRootItemSize(QQuickItem *item)
{
    if (!appRootItem)
        return;

    // The appRootItem always fills the window.
    QQuickItemPrivate *ip = QQuickItemPrivate::get(item);
    if (widthValid(ip)) {
        appRootItem->setWidth(item->width());
    } else {
        appRootItem->setWidth(dd->appWindow->width());
    }

    if (heightValid(ip)) {
        appRootItem->setHeight(item->height());
    } else {
        appRootItem->setWidth(dd->appWindow->height());
    }
}
