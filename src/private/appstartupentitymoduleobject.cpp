#include "appstartupentitymoduleobject.h"
#include "appstartupcomponent.h"
#include "appstartupinstance_p.h"
#include "appstartupdependencyresolver.h"
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
        if (status == QQmlIncubator::Error) {
            QString errorString = "AppStartupInstance incubator error: ";
            for (auto error : this->errors()) {
                errorString += error.toString();
                errorString += "\t";
            }

            qWarning() << errorString;

            Q_EMIT entityModule->qq->errorOccured(entityModule->group(), errorString);

            if (obj)
                obj->deleteLater();
        }
        return;
    }

    entityModule->_childObjects.append(obj);
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
    qDeleteAll(_childObjects);
    _childObjects.clear();
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
        const QString &errorString = "Load the entity module failed!";
        qWarning() << errorString;

        Q_EMIT qq->errorOccured(group(), errorString);
        return false;
    }

    entityInstance = qobject_cast<AppStartupEntityInterface *>(obj);
    if (!entityInstance) {
        const QString &errorString = "Convert the entity module failed!";
        qWarning() << errorString;

        Q_EMIT qq->errorOccured(group(), errorString);
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
            QString errorString = "AppStartupInstance error: ";
            for (auto error : entityComponent->errors()) {
                errorString += error.toString();
                errorString += "\t";
            }
            errorString += qPrintable(entityComponent->errorString());

            qWarning().nospace() << errorString;

            Q_EMIT qq->errorOccured(group(), errorString);
            return false;
        }

        _q_onEntityModuleStatusChanged(entityComponent->status());
    }

    return true;
}

void AppStartupEntityModuleObject::finishedLoaded()
{
    m_startedComponents.clear();
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

    QSharedPointer<AppStartupModuleGroup> module = group();
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
        updateSurfacePopulate(rootItem);
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
        if (status == QQmlComponent::Error) {
            QString errorString = "AppStartupInstance error: ";
            for (auto error : entityComponent->errors()) {
                errorString += error.toString();
                errorString += "\t";
            }
            errorString += qPrintable(entityComponent->errorString());

            qWarning().nospace() << errorString;

            Q_EMIT qq->errorOccured(group(), errorString);
        }
        return;
    }

    QQmlListReference ref = findWindowDefaultDataRef();
    if (!ref.isValid()) {
        if (QQuickItem *containerItem = containerContentItemFromBinder())
            ref = QQmlListReference(containerItem, WINDOW_OR_ITEM_CONTENTDATA);
    }

    bool success = createObjects(ref);
    if (!success) {
        const QString &errorString = "Create entity objects failed!";
        qWarning() << errorString;

        Q_EMIT qq->errorOccured(group(), errorString);
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

void AppStartupEntityModuleObject::_q_onRootSurfaceItemPopulatedChanged(AppStartupItem *surfaceRootItem)
{
    AppStartupItem *rootItem = appRootItem();

    if (surfaceRootItem->populate()) {
        rootItem->setPopulate(true);
    }
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
    _rootItem.reset(qobject_cast<AppStartupItem *>(object));
    setContentItem(_rootItem.get());

    if (entityComponent->isError()) {
        qWarning() << "The entity component create failed: " << entityComponent->errors();
    }

    Q_ASSERT_X(_rootItem, "AppStartupInstance", qPrintable("Create the AppStartupItem item failed!"));

    resovleInterface(_rootItem.get());
    if (QQmlContext *context = transitionGroupContextFromBinder())
        context->setContextProperty(QLatin1String("enterTarget"), _rootItem.get());

    _rootItem->setContainer(containerItem);
    _rootItem->setEnabled(false);
    _rootItem->setVisible(false);

    entityComponent->completeCreate();

    AppPreloadItem *preloadItem = qmlobject_cast<AppPreloadItem *>(binder()->contentItem());
    if (preloadItem) {
        preloadItem->setStartupItem(_rootItem.get());
    }

    pros.append(_rootItem.get());

    initRootItem(containerItem);
    updateRootItemSize(containerItem);
    createChildComponents();
    return true;
}

void AppStartupEntityModuleObject::createChildComponents()
{
    AppStartupItem *rootItem = appRootItem();
    if (!rootItem)
        return;

    m_startedComponents.clear();
    QList<AppStartupComponent *> allComponents;
    auto objects = rootItem->objects().toList<QList<QObject *>>();

    for (QObject *obj : objects) {
        if (AppStartupComponent *component = qobject_cast<AppStartupComponent *>(obj)) {
            allComponents.append(component);
        }
    }

    childrenCount = allComponents.size();
    if (childrenCount == 0) {
        finishedLoaded();
        return;
    }

    auto result = AppStartupDependencyResolver::resolve(allComponents);

    if (!result.success) {
        QString finalError = QString("[AppStartup] Dependency Error: %1").arg(result.errorString);

        if (!result.circularChain.isEmpty()) {
            QString chainStr;
            for (auto comp : result.circularChain) {
                chainStr += (comp->objectName().isEmpty() ? "Unnamed" : comp->objectName()) + " -> ";
            }
            finalError += QString(" | Cycle Chain: %1").arg(chainStr);
        }

        qCritical() << qPrintable(finalError);

        Q_EMIT qq->errorOccured(group(), finalError);
        return;
    }

    qDeleteAll(componentDependencyHash);
    componentDependencyHash.clear();

    for (auto component : allComponents) {
        ComponentDependency *dependency = nullptr;
        if (componentDependencyHash.contains(component)) {
            dependency = componentDependencyHash.value(component);
        } else {
            dependency = new ComponentDependency();
            componentDependencyHash.insert(component, dependency);
        }

        QQmlListProperty<AppStartupComponent> dependsProp = component->depends();
        qsizetype count = dependsProp.count(&dependsProp);
        for (qsizetype i = 0; i < count; ++i) {
            AppStartupComponent *target = dependsProp.at(&dependsProp, i);
            if (target) {
                dependency->dependsOn.insert(target);

                ComponentDependency *targetDependency = nullptr;
                if (componentDependencyHash.contains(target)) {
                    targetDependency = componentDependencyHash.value(target);
                } else {
                    targetDependency = new ComponentDependency();
                    componentDependencyHash.insert(target, targetDependency);
                }
                targetDependency->beingDepends.insert(component);
            }
        }
    }

    if (!result.batches.isEmpty()) {
        const QList<AppStartupComponent*> &layer0 = result.batches.first();

        qInfo() << "[AppStartup] Starting incubation batch 0 with" << layer0.size() << "components.";

        for (auto component : layer0) {
            createComponnet(component);
        }
    } else {
        finishedLoaded();
    }
}

void AppStartupEntityModuleObject::createComponnet(AppStartupComponent *component)
{
    if (m_startedComponents.contains(component)) {
        return;
    }

    m_startedComponents.insert(component);
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

void AppStartupEntityModuleObject::updateSurfacePopulate(AppStartupItem *rootItem)
{
    if (auto surfaceItem = binder()->appSurfaceItem()) {
        // surface item need to wait until the root startup item finished
        AppStartupItemAttached *attached = qobject_cast<AppStartupItemAttached*>(qmlAttachedPropertiesObject<AppStartupItem>(surfaceItem, true));
        if (attached && attached->startupItem()) {
            AppStartupItem *surfaceRootItem = attached->startupItem();
            if (!surfaceRootItem || surfaceRootItem->populate()) {
                rootItem->setPopulate(true);
            } else {
                QObject::connect(surfaceRootItem, &AppStartupItem::populateChanged, rootItem, std::bind(&AppStartupEntityModuleObject::_q_onRootSurfaceItemPopulatedChanged, this, surfaceRootItem), Qt::SingleShotConnection);
            }
        } else {
            rootItem->setPopulate(true);
        }
    } else {
        rootItem->setPopulate(true);
    }
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
