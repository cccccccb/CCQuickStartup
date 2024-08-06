#include "appstartupentitycomponent.h"
#include "appstartupinstance_p.h"
#include "defines_p.h"

#include "interface/appstartupentityinterface.h"
#include "items/appstartupinitialproperties.h"
#include "items/appstartupitemattached.h"
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
    AppQmlComponentIncubator(QQmlComponent *component, AppStartupEntityComponent *appExtra,
                             IncubationMode mode = AsynchronousIfNested);

protected:
    virtual void statusChanged(Status) override;
    virtual void setInitialState(QObject *) override;

private:
    AppStartupEntityComponent *entityComponent;
    QQmlComponent *compnent;
};

AppQmlComponentIncubator::AppQmlComponentIncubator(QQmlComponent *component, AppStartupEntityComponent *entityComponent, QQmlIncubator::IncubationMode mode)
    : QQmlIncubator(mode)
    , entityComponent(entityComponent)
    , compnent(component)
{

}

void AppQmlComponentIncubator::statusChanged(QQmlIncubator::Status status)
{
    QObject *obj = object();
    if (status != QQmlIncubator::Ready) {
        if (status == QQmlIncubator::Error)
            qWarning() << "AppStartupInstance: " << this->errors();
        return;
    }

    AppStartupItemAttached *itemAttached = qobject_cast<AppStartupItemAttached*>(
        qmlAttachedPropertiesObject<AppStartupItem>(entityComponent->appRootItem, true));
    if (itemAttached) {
        QQmlContext *context = qmlContext(obj);
        itemAttached->insert(context->nameForObject(obj), QVariant::fromValue(obj));
    }

    entityComponent->_q_onComponentProgressChanged();
    entityComponent->destoryIncubator(this);
}

void AppQmlComponentIncubator::setInitialState(QObject *o)
{
    if (!entityComponent->appRootItem)
        return;

    if (o) {
        QQmlContext *context = entityComponent->itemContextMap.value(compnent);
        if (context)
            QQml_setParent_noEvent(context, o);
        QQml_setParent_noEvent(o, entityComponent->appRootItem);
        if (QQuickItem *item = qmlobject_cast<QQuickItem *>(o))
            item->setParentItem(entityComponent->appRootItem);
    }
}

AppStartupEntityComponent::~AppStartupEntityComponent()
{
    if (dd->windowContentItem)
        deinitRootInit(dd->windowContentItem);
}

QQuickItem *AppStartupEntityComponent::transitionItem()
{
    return appRootItem;
}

QQuickTransition *AppStartupEntityComponent::transition()
{
    return transitionGroup ? transitionGroup->enter() : nullptr;
}

AppStartupComponent *AppStartupEntityComponent::transitionLinkPrev()
{
    return binder();
}

void AppStartupEntityComponent::transitionFinish()
{
    endOfTransition();
    if (transitionGroup && !initialPropertiesHash.isEmpty()) {
        initialItemProperties(appRootItem, initialPropertiesHash);
        initialPropertiesHash.clear();
    }
}

void AppStartupEntityComponent::beforeTransition()
{
    if (transitionGroup)
        initialPropertiesHash = initialItemProperties(appRootItem, transitionGroup->enterInitialProperties());
}

bool AppStartupEntityComponent::load()
{
    const AppStartupComponentInformation &pluginInfo = this->information;
    QPluginLoader loader(pluginInfo.path());
    entityInstance.reset(qobject_cast<AppStartupEntityInterface *>(loader.instance()));
    if (!entityInstance) {
        //! @todo add error
        qWarning("Load the entity plugin failed!");
        return false;
    }

    entityInstance->initialize(dd->engine.get());
    // Insert component into preload
    const QUrl &entityComponentPath = entityInstance->entityComponentPath();
    entityComponent = new QQmlComponent(dd->engine.get(), entityComponentPath, QQmlComponent::Asynchronous);
    if (entityComponent->isLoading()) {
        QObject::connect(entityComponent, &QQmlComponent::statusChanged,
                         this, &AppStartupEntityComponent::_q_onEntityComponentStatusChanged);
    } else {
        if (entityComponent->status() == QQmlComponent::Error) {
            qWarning() << "AppStartupInstance: " << entityComponent->errors() << ", " << entityComponent->errorString();
            return false;
        }

        _q_onEntityComponentStatusChanged(entityComponent->status());
    }

    return true;
}

void AppStartupEntityComponent::finishedLoaded()
{
    entityInstance->finishedLoading(dd->engine.get());
    appRootItem->setLoaded(true);

    AppStartupItemAttached *itemAttached = qobject_cast<AppStartupItemAttached*>(qmlAttachedPropertiesObject<AppStartupItem>(appRootItem, true));
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

void AppStartupEntityComponent::endOfTransition()
{
    if (appRootItem) {
        appRootItem->setEnabled(true);
        appRootItem->setFocus(true);
        appRootItem->setVisible(true);
        appRootItem->setPopulate(true);
    }
}

void AppStartupEntityComponent::destoryIncubator(QQmlIncubator *incubator)
{
    incubators.removeAll(incubator);
    childrenCount--;

    if (childrenCount == 0)
        finishedLoaded();

    incubator->clear();
    delete incubator;
}

void AppStartupEntityComponent::_q_onEntityComponentStatusChanged(QQmlComponent::Status status)
{
    if (status != QQmlComponent::Ready) {
        if (status == QQmlComponent::Error)
            qWarning() << "AppStartupInstance: " << entityComponent->errors() << ", " << entityComponent->errorString();
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

    entityComponent->deleteLater();
    entityComponent = nullptr;
}

void AppStartupEntityComponent::_q_onComponentProgressChanged()
{
    qreal progress = 0;
    auto components = appRootItem->findChildren<QQmlComponent *>();
    for (auto childCom : std::as_const(components)) {
        progress += childCom->progress();
    }

    appRootItem->setProgress(progress / components.count());
}

void AppStartupEntityComponent::itemGeometryChanged(QQuickItem *item, QQuickGeometryChange change, const QRectF &oldGeometry)
{
    updateRootItemSize(item);
    QQuickItemChangeListener::itemGeometryChanged(item, change, oldGeometry);
}

bool AppStartupEntityComponent::createObjects(const char *propertyName)
{
    Q_ASSERT(entityComponent);
    QQmlListReference pros(dd->appWindow, propertyName);
    if (!pros.isValid())
        return false;

    copyTransitionGroupFromBinder();
    QObject *object = entityComponent->beginCreate(creationContext(entityComponent, dd->appWindow));
    appRootItem = qobject_cast<AppStartupItem *>(object);
    if (entityComponent->isError()) {
        qWarning() << "The entity component create failed: " << entityComponent->errors();
    }

    Q_ASSERT_X(appRootItem, "AppStartupInstance", qPrintable("Create the AppStartupItem item failed!"));

    if (QQmlContext *context = transitionGroupContextFromBinder())
        context->setContextProperty(QLatin1String("enterTarget"), appRootItem);

    appRootItem->setContainWindow(dd->appWindow);
    appRootItem->setEnabled(false);
    appRootItem->setVisible(false);

    entityComponent->completeCreate();

    if (dd->defaultComponentGroup.entity() == this->information)
        dd->defaultAppStartItem = appRootItem;

    AppStartupPreloadAttached *preloadAttached = qobject_cast<AppStartupPreloadAttached*>(qmlAttachedPropertiesObject<AppStartupPreloadAttached>(dd->appWindow, false));
    if ((dd->defaultComponentGroup.entity() == this->information) && preloadAttached) {
        preloadAttached->setStartupItem(appRootItem);
    }

    pros.append(appRootItem);
    initRootItem(dd->windowContentItem);
    updateRootItemSize(dd->windowContentItem);
    createChildComponents();
    return true;
}

void AppStartupEntityComponent::createChildComponents()
{
    auto components = appRootItem->findChildren<QQmlComponent *>(QStringLiteral(""), Qt::FindDirectChildrenOnly);
    childrenCount = components.size();

    if (childrenCount == 0) {
        finishedLoaded();
        return;
    }

    for (auto childCom : std::as_const(components)) {
        QObject::connect(childCom, &QQmlComponent::progressChanged, this,
                         &AppStartupEntityComponent::_q_onComponentProgressChanged);
        auto asyn = appRootItem->asynchronous() ? AppQmlComponentIncubator::Asynchronous : AppQmlComponentIncubator::AsynchronousIfNested;
        AppQmlComponentIncubator *incubator = new AppQmlComponentIncubator(childCom, this, asyn);
        this->incubators.append(incubator);
        childCom->create(*incubator, creationContext(childCom, appRootItem));
    }
}

void AppStartupEntityComponent::updateRootItemSize(QQuickItem *item)
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
