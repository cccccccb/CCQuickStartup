#include "appstartupentitymodule.h"
#include "appstartupcomponent.h"
#include "appstartupinstance_p.h"
#include "defines_p.h"

#include "interface/appstartupentityinterface.h"
#include "items/appstartupinitialproperties.h"
#include "items/appstartupitemattached.h"
#include "items/appstartuptransitiongroup.h"
#include "items/appstartupitem.h"
#include "items/appstartuppreloaditem.h"

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
                             AppStartupEntityModule *appExtra, IncubationMode mode = AsynchronousIfNested);

protected:
    virtual void statusChanged(Status) override;
    virtual void setInitialState(QObject *) override;

private:
    AppStartupEntityModule *entityModule;
    AppStartupComponent *startupComponent;
    QQmlComponent *compnent;
};

AppQmlComponentIncubator::AppQmlComponentIncubator(AppStartupComponent *startupComponent, QQmlComponent *component,
                                                   AppStartupEntityModule *entityModule, QQmlIncubator::IncubationMode mode)
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
    AppStartupEntityModule::ComponentDependency *dependency = entityModule->componentDependencyHash.value(startupComponent);
    if (dependency && !dependency->beingDepends.isEmpty()) {
        for (auto beingDepends : dependency->beingDepends) {
            AppStartupEntityModule::ComponentDependency *beingDependsDependency = entityModule->componentDependencyHash.value(beingDepends);
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

AppStartupEntityModule::~AppStartupEntityModule()
{
    qDebug() << "App startup entity component destruction";
    entityInstance = nullptr;
}

QQuickItem *AppStartupEntityModule::transitionItem()
{
    return contentItem();
}

QQuickTransition *AppStartupEntityModule::transition()
{
    return _transitionGroup ? _transitionGroup->enter() : nullptr;
}

AppStartupModule *AppStartupEntityModule::transitionLinkPrev()
{
    return binder();
}

void AppStartupEntityModule::transitionFinish()
{
    endOfTransition();
    if (_transitionGroup && !initialPropertiesHash.isEmpty()) {
        AppStartupItem *rootItem = appRootItem();
        initialItemProperties(rootItem, initialPropertiesHash);
        initialPropertiesHash.clear();
    }
}

void AppStartupEntityModule::beforeTransition()
{
    AppStartupItem *rootItem = appRootItem();
    if (_transitionGroup && rootItem)
        initialPropertiesHash = initialItemProperties(rootItem, _transitionGroup->enterInitialProperties());
}

bool AppStartupEntityModule::load()
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
                         this, &AppStartupEntityModule::_q_onEntityModuleStatusChanged);
    } else {
        if (entityComponent->status() == QQmlComponent::Error) {
            qWarning() << "AppStartupInstance: " << entityComponent->errors() << ", " << qPrintable(entityComponent->errorString());
            return false;
        }

        _q_onEntityModuleStatusChanged(entityComponent->status());
    }

    return true;
}

void AppStartupEntityModule::finishedLoaded()
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

    AppStartupModuleGroup module({binder()->information(), this->information()});
    dd->loadedModulesList += module;
    Q_EMIT qq->loaded(module);

    for (auto it = dd->reloadModulesList.begin(); it != dd->reloadModulesList.end();) {
        dd->loadPreloadModules(*it);
        it = dd->reloadModulesList.erase(it);
    }
}

void AppStartupEntityModule::endOfTransition()
{
    AppStartupItem *rootItem = appRootItem();

    if (rootItem) {
        rootItem->setEnabled(true);
        rootItem->setFocus(true);
        rootItem->setVisible(true);
        rootItem->setPopulate(true);
    }
}

AppStartupItem *AppStartupEntityModule::appRootItem() const
{
    if (contentItem().isNull())
        return nullptr;

    return qmlobject_cast<AppStartupItem *>(contentItem());
}

void AppStartupEntityModule::destoryIncubator(QQmlIncubator *incubator)
{
    incubators.removeAll(incubator);
    childrenCount--;

    if (childrenCount == 0)
        finishedLoaded();

    incubator->clear();
    delete incubator;
}

void AppStartupEntityModule::_q_onEntityModuleStatusChanged(QQmlComponent::Status status)
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

void AppStartupEntityModule::_q_onComponentProgressChanged()
{
    AppStartupItem *rootItem = appRootItem();
    qreal progress = 0;
    auto components = rootItem->findChildren<QQmlComponent *>();

    for (auto childCom : std::as_const(components)) {
        progress += childCom->progress();
    }

    rootItem->setProgress(progress / components.count());
}

void AppStartupEntityModule::itemGeometryChanged(QQuickItem *item, QQuickGeometryChange change, const QRectF &oldGeometry)
{
    updateRootItemSize(item);
    QQuickItemChangeListener::itemGeometryChanged(item, change, oldGeometry);
}

bool AppStartupEntityModule::createObjects(const QQmlListReference &pros)
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

void AppStartupEntityModule::createChildComponents()
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

void AppStartupEntityModule::createComponnet(AppStartupComponent *component)
{
    QQmlComponent *targetComponent = component->target();
    QObject::connect(targetComponent, &QQmlComponent::progressChanged, this,
                     &AppStartupEntityModule::_q_onComponentProgressChanged);

    AppStartupItem *rootItem = appRootItem();
    auto asyn = rootItem->asynchronous() ? AppQmlComponentIncubator::Asynchronous : AppQmlComponentIncubator::AsynchronousIfNested;
    AppQmlComponentIncubator *incubator = new AppQmlComponentIncubator(component, targetComponent, this, asyn);
    this->incubators.append(incubator);
    targetComponent->create(*incubator, creationContext(targetComponent, rootItem));
}

QQmlListReference AppStartupEntityModule::findWindowDefaultDataRef()
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

void AppStartupEntityModule::updateRootItemSize(QQuickItem *item)
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
