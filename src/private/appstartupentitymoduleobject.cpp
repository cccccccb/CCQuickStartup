#include "appstartupentitymoduleobject.h"
#include "appstartupcomponent.h"
#include "appstartupinstance_p.h"
#include "defines_p.h"

#include "interface/appstartupentityinterface.h"
#include "items/appstartupitemattached.h"
#include "items/appstartuptransitiongroup.h"
#include "items/appstartupitem.h"
#include "items/appstartuppreloaditem.h"
#include "items/appstartupinterface.h"

#include <QPluginLoader>
#include <QQmlIncubator>
#include <QQmlComponent>
#include <QQmlApplicationEngine>

#include <private/qquicktransition_p.h>

template<typename T, typename U = std::remove_pointer<T>>
static QSet<T> qmlListPropertyToSet(const QQmlListProperty<U> &qmlList)
{
    QQmlListProperty<U> temp(qmlList);
    const qsizetype size = temp.count(&temp);

    QSet<T> result;
    result.reserve(size);

    for (qsizetype i = 0; i < size; ++i)
        result.insert(temp.at(&temp, i));

    return result;
}

class AppQmlComponentIncubator : public QQmlIncubator
{
public:
    AppQmlComponentIncubator(AppStartupComponent *startupComponent, QQmlComponent *component,
                             AppStartupEntityModuleObject *appExtra, IncubationMode mode = AsynchronousIfNested);

protected:
    virtual void statusChanged(Status) override;
    virtual void setInitialState(QObject *) override;

private:
    AppStartupEntityModuleObject *entityModule;
    AppStartupComponent *startupComponent;
    QQmlComponent *compnent;
};

AppQmlComponentIncubator::AppQmlComponentIncubator(AppStartupComponent *startupComponent, QQmlComponent *component,
                                                   AppStartupEntityModuleObject *entityModule, QQmlIncubator::IncubationMode mode)
    : QQmlIncubator(mode)
    , startupComponent(startupComponent)
    , entityModule(entityModule)
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

    AppStartupItemAttached *itemAttached = qobject_cast<AppStartupItemAttached*>(qmlAttachedPropertiesObject<AppStartupItem>(entityModule->contentItem(), true));
    if (itemAttached) {
        QQmlContext *context = qmlContext(obj);
        itemAttached->insert(context->nameForObject(obj), QVariant::fromValue(obj));
    }

    entityModule->_q_onComponentProgressChanged();

    QList<AppStartupComponent *> readyCreateComponents;
    AppStartupEntityModuleObject::ComponentDependency *dependency = entityModule->componentDependencyHash.value(startupComponent);
    if (dependency && !dependency->beingDepends.isEmpty()) {
        for (auto beingDepends : dependency->beingDepends) {
            AppStartupEntityModuleObject::ComponentDependency *beingDependsDependency = entityModule->componentDependencyHash.value(beingDepends);
            if (!beingDependsDependency)
                continue;

            bool allFinished = std::all_of(beingDependsDependency->dependsOn.begin(), beingDependsDependency->dependsOn.end(), [](AppStartupComponent *component) {
                return !component->target() || component->target()->isReady();
            });

            if (allFinished)
                readyCreateComponents << beingDepends;
        }
    }

    for (auto component : readyCreateComponents) {
        entityModule->createComponnet(component);
    }

    entityModule->destoryIncubator(this);
}

void AppQmlComponentIncubator::setInitialState(QObject *o)
{
    if (!entityModule->contentItem())
        return;

    if (o) {
        QQmlContext *context = entityModule->_itemContextMap.value(compnent);
        if (context)
            QQml_setParent_noEvent(context, o);

        QQml_setParent_noEvent(o, entityModule->contentItem());

        if (QQuickItem *item = qmlobject_cast<QQuickItem *>(o))
            item->setParentItem(entityModule->contentItem());
        else {
            QQmlProperty prop(o, PARENT_PROPERTY_NAME, context ? context : qmlContext(o));

            if (prop.isValid() && prop.isWritable()) {
                prop.write(QVariant::fromValue<QQuickItem *>(entityModule->contentItem()));
            }
        }

        entityModule->initialItemProperties(o, startupComponent->initialProperties());
    }
}

AppStartupEntityModuleObject::~AppStartupEntityModuleObject()
{
    qDebug() << "App startup entity component destruction";
    entityInstance = nullptr;
}

QQuickItem *AppStartupEntityModuleObject::transitionItem()
{
    return contentItem();
}

QQuickTransition *AppStartupEntityModuleObject::transition()
{
    return _transitionGroup ? _transitionGroup->enter() : nullptr;
}

AppStartupModuleObject *AppStartupEntityModuleObject::transitionLinkPrev()
{
    return binder();
}

void AppStartupEntityModuleObject::transitionFinish()
{
    endOfTransition();
    if (_transitionGroup && !initialPropertiesHash.isEmpty()) {
        AppStartupItem *rootItem = appRootItem();
        initialItemProperties(rootItem, initialPropertiesHash);
        initialPropertiesHash.clear();
    }
}

void AppStartupEntityModuleObject::beforeTransition()
{
    AppStartupItem *rootItem = appRootItem();
    if (_transitionGroup && rootItem)
        initialPropertiesHash = initialItemProperties(rootItem, _transitionGroup->enterInitialProperties());
}

bool AppStartupEntityModuleObject::load()
{
    QObject *obj = this->loadModule(this->_information.path());
    if (!obj) {
        qFatal("Load the entity module failed!");
        return false;
    }

    entityInstance = qobject_cast<AppStartupEntityInterface *>(obj);
    if (!entityInstance) {
        //! @todo add error
        qFatal("Convert the entity module failed!");
        return false;
    }

    entityInstance->initialize(dd->engine.get());
    // Insert component into preload
    const QUrl &entityModulePath = entityInstance->entityModulePath();
    entityComponent = new QQmlComponent(dd->engine.get(), entityModulePath, QQmlComponent::Asynchronous);
    if (entityComponent->isLoading()) {
        QObject::connect(entityComponent, &QQmlComponent::statusChanged,
                         this, &AppStartupEntityModuleObject::_q_onEntityModuleStatusChanged);
    } else {
        if (entityComponent->status() == QQmlComponent::Error) {
            qWarning() << "AppStartupInstance: " << entityComponent->errors() << ", " << qPrintable(entityComponent->errorString());
            return false;
        }

        _q_onEntityModuleStatusChanged(entityComponent->status());
    }

    return true;
}

void AppStartupEntityModuleObject::finishedLoaded()
{
    AppStartupItem *rootItem = appRootItem();
    entityInstance->finishedLoading(dd->engine.get());
    rootItem->setLoaded(true);

    AppStartupItemAttached *itemAttached = qobject_cast<AppStartupItemAttached*>(qmlAttachedPropertiesObject<AppStartupItem>(rootItem, true));
    if (itemAttached)
        itemAttached->setLoaded(true);

    AppPreloadItem *preloadItem = qmlobject_cast<AppPreloadItem *>(binder()->contentItem());
    if (preloadItem)
        preloadItem->setLoaded(true);

    if (!preloadItem || preloadItem->autoExitOverlay())
        startTransition();

    rootItem->setFocus(true);

    QSharedPointer<AppStartupModuleGroup> module(new AppStartupModuleGroup({binder()->information(), this->information()}, qq));
    dd->loadedModulesList += module;
    Q_EMIT qq->loaded(module);

    for (auto it = dd->reloadModulesList.begin(); it != dd->reloadModulesList.end();) {
        QSharedPointer<AppStartupModuleGroup> group(*it);
        it = dd->reloadModulesList.erase(it);
        dd->loadPreloadModules(group);
    }

    qDeleteAll(componentDependencyHash);
    componentDependencyHash.clear();
}

void AppStartupEntityModuleObject::endOfTransition()
{
    AppStartupItem *rootItem = appRootItem();

    if (rootItem) {
        rootItem->setEnabled(true);
        rootItem->setFocus(true);
        rootItem->setVisible(true);
        rootItem->setPopulate(true);
    }
}

AppStartupItem *AppStartupEntityModuleObject::appRootItem() const
{
    if (contentItem().isNull())
        return nullptr;

    return qmlobject_cast<AppStartupItem *>(contentItem());
}

void AppStartupEntityModuleObject::destoryIncubator(QQmlIncubator *incubator)
{
    incubators.removeOne(incubator);
    childrenCount--;

    if (childrenCount == 0)
        finishedLoaded();

    incubator->clear();
    delete incubator;
}

void AppStartupEntityModuleObject::_q_onEntityModuleStatusChanged(QQmlComponent::Status status)
{
    if (status != QQmlComponent::Ready) {
        if (status == QQmlComponent::Error)
            qWarning() << "AppStartupInstance: " << entityComponent->errors() << ", " << qPrintable(entityComponent->errorString());
        return;
    }

    QQmlListReference ref = findWindowDefaultDataRef();
    if (!ref.isValid()) {
        if (QQuickItem *containerItem = containerContentItemFromBinder())
            ref = QQmlListReference(containerItem, WINDOW_OR_ITEM_CONTENTDATA);
    }

    bool success = createObjects(ref);
    if (!success) {
        qWarning() << "Create entity objects failed!";
    }

    entityComponent->deleteLater();
    entityComponent = nullptr;
}

void AppStartupEntityModuleObject::_q_onComponentProgressChanged()
{
    AppStartupItem *rootItem = appRootItem();
    qreal progress = 0;
    auto components = rootItem->findChildren<QQmlComponent *>();

    for (auto childCom : std::as_const(components)) {
        progress += childCom->progress();
    }

    rootItem->setProgress(progress / components.count());
}

void AppStartupEntityModuleObject::resovleInterface(AppStartupItem *rootItem)
{
    if (!rootItem)
        return;

    auto interfaces = rootItem->interfaces().toList<QList<AppStartupInterface *>>();
    QVariantList variantList;
    std::transform(interfaces.begin(), interfaces.end(), std::back_inserter(variantList), [](AppStartupInterface *interface) {
        return QVariant::fromValue<AppStartupInterface *>(interface);
    });

    insertModuleBindingPropertyList(AppStartupModuleGroup::ResolvedInterface, variantList);
}

void AppStartupEntityModuleObject::itemGeometryChanged(QQuickItem *item, QQuickGeometryChange change, const QRectF &oldGeometry)
{
    updateRootItemSize(item);
    QQuickItemChangeListener::itemGeometryChanged(item, change, oldGeometry);
}

bool AppStartupEntityModuleObject::createObjects(const QQmlListReference &pros)
{
    Q_ASSERT(entityComponent);
    if (!pros.isValid())
        return false;

    QQuickItem *containerItem = containerContentItemFromBinder();
    if (!containerItem)
        return false;

    copyTransitionGroupFromBinder();
    QObject *object = entityComponent->beginCreate(creationContext(entityComponent, containerItem));
    AppStartupItem *rootItem = qobject_cast<AppStartupItem *>(object);
    setContentItem(rootItem);

    if (entityComponent->isError()) {
        qWarning() << "The entity component create failed: " << entityComponent->errors();
    }

    Q_ASSERT_X(rootItem, "AppStartupInstance", qPrintable("Create the AppStartupItem item failed!"));

    resovleInterface(rootItem);
    if (QQmlContext *context = transitionGroupContextFromBinder())
        context->setContextProperty(QLatin1String("enterTarget"), rootItem);

    rootItem->setContainer(containerItem);
    rootItem->setEnabled(false);
    rootItem->setVisible(false);

    entityComponent->completeCreate();

    AppPreloadItem *preloadItem = qmlobject_cast<AppPreloadItem *>(binder()->contentItem());
    if (preloadItem) {
        preloadItem->setStartupItem(rootItem);
    }

    pros.append(rootItem);

    initRootItem(containerItem);
    updateRootItemSize(containerItem);
    createChildComponents();
    return true;
}

void AppStartupEntityModuleObject::createChildComponents()
{
    AppStartupItem *rootItem = appRootItem();

    auto objects = rootItem->objects().toList<QList<QObject *>>();
    std::for_each(objects.begin(), objects.end(), [this](QObject *obj) {
        if (AppStartupComponent *component = qobject_cast<AppStartupComponent *>(obj)) {
            auto dependsOn = qmlListPropertyToSet<AppStartupComponent *>(component->depends());
            ComponentDependency *dependency = nullptr;
            if (componentDependencyHash.contains(component)) {
                dependency = componentDependencyHash.value(component);
            } else {
                dependency = new ComponentDependency();
            }

            for (auto depends : dependsOn)
                dependency->dependsOn << depends;

            componentDependencyHash.insert(component, dependency);

            for (auto dependsComponent : dependsOn) {
                ComponentDependency *dependency = nullptr;
                if (componentDependencyHash.contains(dependsComponent)) {
                    dependency = componentDependencyHash.value(dependsComponent);
                } else {
                    dependency = new ComponentDependency();
                }

                dependency->beingDepends << component;
                componentDependencyHash.insert(dependsComponent, dependency);
            }
        }
    });

    childrenCount = componentDependencyHash.size();

    if (childrenCount == 0) {
        finishedLoaded();
        return;
    }

    // find the non dependency component
    QVector<AppStartupComponent *> components;
    std::for_each(componentDependencyHash.keyValueBegin(), componentDependencyHash.keyValueEnd(),
                                    [&components](std::pair<AppStartupComponent *, ComponentDependency *> keyValuePair) {
        if (keyValuePair.second->dependsOn.isEmpty()) {
            components << keyValuePair.first;
        }

        auto intersected = keyValuePair.second->dependsOn.intersect(keyValuePair.second->beingDepends);
        if (!intersected.isEmpty()) {
            qFatal() << "[App Startup] component depends loop: " << intersected << ", with " << keyValuePair.first;
            return;
        }
    });

    if (components.isEmpty()) {
        qFatal() << "[App Startup]  component is empty or component depends loop!";
        return;
    }

    for (auto childCom : std::as_const(components)) {
        createComponnet(childCom);
    }
}

void AppStartupEntityModuleObject::createComponnet(AppStartupComponent *component)
{
    QQmlComponent *targetComponent = component->target();
    QObject::connect(targetComponent, &QQmlComponent::progressChanged, this,
                     &AppStartupEntityModuleObject::_q_onComponentProgressChanged);

    AppStartupItem *rootItem = appRootItem();
    auto asyn = rootItem->asynchronous() ? AppQmlComponentIncubator::Asynchronous : AppQmlComponentIncubator::AsynchronousIfNested;
    AppQmlComponentIncubator *incubator = new AppQmlComponentIncubator(component, targetComponent, this, asyn);
    this->incubators.append(incubator);
    targetComponent->create(*incubator, creationContext(targetComponent, rootItem));
}

QQmlListReference AppStartupEntityModuleObject::findWindowDefaultDataRef()
{
    QQuickWindow *window = appWindowFromBinder();
    if (!window)
        return {};

    QQmlListReference pros(window, APPLICATIONWINDOW_CONTENTDATA);
    if (pros.isValid())
        return pros;

    pros = QQmlListReference(window, WINDOW_OR_ITEM_CONTENTDATA);
    return pros;
}

void AppStartupEntityModuleObject::updateRootItemSize(QQuickItem *item)
{
    AppStartupItem *rootItem = appRootItem();
    if (!rootItem)
        return;    

    // The appRootItem always fills the container.
    QQuickItemPrivate *ip = QQuickItemPrivate::get(item);
    if (widthValid(ip)) {
        rootItem->setWidth(item->width());
    }

    if (heightValid(ip)) {
        rootItem->setHeight(item->height());
    }
}
