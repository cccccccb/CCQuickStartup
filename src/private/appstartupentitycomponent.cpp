#include "appstartupentitycomponent.h"
#include "appstartupinstance_p.h"
#include "defines_p.h"

#include "interface/appstartupentityinterface.h"
#include "items/appstartupinitialproperties.h"
#include "items/appstartupitemattached.h"
#include "items/appstartuptransitiongroup.h"
#include "items/appstartupitem.h"
#include "items/apppreloaditem.h"

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
        qmlAttachedPropertiesObject<AppStartupItem>(entityComponent->contentItem(), true));
    if (itemAttached) {
        QQmlContext *context = qmlContext(obj);
        itemAttached->insert(context->nameForObject(obj), QVariant::fromValue(obj));
    }

    entityComponent->_q_onComponentProgressChanged();
    entityComponent->destoryIncubator(this);
}

void AppQmlComponentIncubator::setInitialState(QObject *o)
{
    if (!entityComponent->contentItem())
        return;

    if (o) {
        QQmlContext *context = entityComponent->itemContextMap.value(compnent);
        if (context)
            QQml_setParent_noEvent(context, o);
        QQml_setParent_noEvent(o, entityComponent->contentItem());
        if (QQuickItem *item = qmlobject_cast<QQuickItem *>(o))
            item->setParentItem(entityComponent->contentItem());
    }
}

AppStartupEntityComponent::~AppStartupEntityComponent()
{
    if (dd->windowContentItem)
        deinitRootInit(dd->windowContentItem);
}

QQuickItem *AppStartupEntityComponent::transitionItem()
{
    return contentItem();
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
        AppStartupItem *rootItem = appRootItem();
        initialItemProperties(rootItem, initialPropertiesHash);
        initialPropertiesHash.clear();
    }
}

void AppStartupEntityComponent::beforeTransition()
{
    AppStartupItem *rootItem = appRootItem();
    if (transitionGroup && rootItem)
        initialPropertiesHash = initialItemProperties(rootItem, transitionGroup->enterInitialProperties());
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
            qWarning() << "AppStartupInstance: " << entityComponent->errors() << ", " << qPrintable(entityComponent->errorString());
            return false;
        }

        _q_onEntityComponentStatusChanged(entityComponent->status());
    }

    return true;
}

void AppStartupEntityComponent::finishedLoaded()
{
    AppStartupItem *rootItem = appRootItem();
    entityInstance->finishedLoading(dd->engine.get());
    rootItem->setLoaded(true);

    AppStartupItemAttached *itemAttached = qobject_cast<AppStartupItemAttached*>(qmlAttachedPropertiesObject<AppStartupItem>(rootItem, true));
    if (itemAttached)
        itemAttached->setLoaded(true);

    if (rootItem == dd->defaultAppStartItem) {
        AppPreloadItem *preloadItem = qmlobject_cast<AppPreloadItem *>(binder()->contentItem());
        if (preloadItem)
            preloadItem->setLoaded(true);

        Q_EMIT qq->loadFinished();

        if (!preloadItem || preloadItem->autoExitOverlay())
            startTransition();

        rootItem->setFocus(true);
    }
}

void AppStartupEntityComponent::endOfTransition()
{
    AppStartupItem *rootItem = appRootItem();

    if (rootItem) {
        rootItem->setEnabled(true);
        rootItem->setFocus(true);
        rootItem->setVisible(true);
        rootItem->setPopulate(true);
    }
}

AppStartupItem *AppStartupEntityComponent::appRootItem() const
{
    if (contentItem().isNull())
        return nullptr;

    return qmlobject_cast<AppStartupItem *>(contentItem());
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
            qWarning() << "AppStartupInstance: " << entityComponent->errors() << ", " << qPrintable(entityComponent->errorString());
        return;
    }

    do {
        // ApplicationWindow.
        if (createObjects(APPLICATIONWINDOW_CONTENTDATA))
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
    AppStartupItem *rootItem = appRootItem();
    qreal progress = 0;
    auto components = rootItem->findChildren<QQmlComponent *>();

    for (auto childCom : std::as_const(components)) {
        progress += childCom->progress();
    }

    rootItem->setProgress(progress / components.count());
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
    setContentItem(qobject_cast<AppStartupItem *>(object));

    if (entityComponent->isError()) {
        qWarning() << "The entity component create failed: " << entityComponent->errors();
    }

    AppStartupItem *rootItem = appRootItem();
    Q_ASSERT_X(rootItem, "AppStartupInstance", qPrintable("Create the AppStartupItem item failed!"));

    if (QQmlContext *context = transitionGroupContextFromBinder())
        context->setContextProperty(QLatin1String("enterTarget"), rootItem);

    rootItem->setContainWindow(dd->appWindow);
    rootItem->setEnabled(false);
    rootItem->setVisible(false);

    entityComponent->completeCreate();

    if (dd->defaultComponentGroup.entity() == this->information)
        dd->defaultAppStartItem = rootItem;

    AppPreloadItem *preloadItem = qmlobject_cast<AppPreloadItem *>(binder()->contentItem());
    if ((dd->defaultComponentGroup.entity() == this->information) && preloadItem) {
        preloadItem->setStartupItem(rootItem);
    }

    pros.append(rootItem);
    initRootItem(dd->windowContentItem);
    updateRootItemSize(dd->windowContentItem);
    createChildComponents();
    return true;
}

void AppStartupEntityComponent::createChildComponents()
{
    AppStartupItem *rootItem = appRootItem();
    auto components = rootItem->findChildren<QQmlComponent *>(QStringLiteral(""), Qt::FindDirectChildrenOnly);
    childrenCount = components.size();

    if (childrenCount == 0) {
        finishedLoaded();
        return;
    }

    for (auto childCom : std::as_const(components)) {
        QObject::connect(childCom, &QQmlComponent::progressChanged, this,
                         &AppStartupEntityComponent::_q_onComponentProgressChanged);
        auto asyn = rootItem->asynchronous() ? AppQmlComponentIncubator::Asynchronous : AppQmlComponentIncubator::AsynchronousIfNested;
        AppQmlComponentIncubator *incubator = new AppQmlComponentIncubator(childCom, this, asyn);
        this->incubators.append(incubator);
        childCom->create(*incubator, creationContext(childCom, rootItem));
    }
}

void AppStartupEntityComponent::updateRootItemSize(QQuickItem *item)
{
    AppStartupItem *rootItem = appRootItem();
    if (!rootItem)
        return;

    // The appRootItem always fills the window.
    QQuickItemPrivate *ip = QQuickItemPrivate::get(item);
    if (widthValid(ip)) {
        rootItem->setWidth(item->width());
    } else {
        rootItem->setWidth(dd->appWindow->width());
    }

    if (heightValid(ip)) {
        rootItem->setHeight(item->height());
    } else {
        rootItem->setWidth(dd->appWindow->height());
    }
}
