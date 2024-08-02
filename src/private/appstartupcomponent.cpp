#include "appstartupcomponent.h"
#include "appstartupinstance_p.h"
#include "appstartuptransitionmanager.h"
#include "items/appstartupitem.h"
#include "items/appstartuppreloadattached.h"
#include "items/appstartupinitialproperties.h"
#include "items/appstartuptransitiongroup.h"

#include <private/qquickitem_p.h>


static const QQuickItemPrivate::ChangeTypes changedTypes = QQuickItemPrivate::Geometry;

AppStartupComponent::AppStartupComponent(const AppStartupComponentInformation &plugin, AppStartupInstancePrivate *dd)
    : qq(dd->qq)
    , dd(dd)
    , information(plugin)
    , duringTransition(false)
{

}

AppStartupComponent::~AppStartupComponent()
{
    qDeleteAll(itemContextMap.values());
    itemContextMap.clear();

    if (transitionManager) {
        if (transitionManager->isRunning())
            transitionManager->cancel();

        delete transitionManager;
        transitionManager = nullptr;
    }
}

QQmlContext *AppStartupComponent::creationContext(QQmlComponent *component, QObject *obj)
{
    QQmlContext *creationContext = component->creationContext();
    if (!creationContext)
        creationContext = qmlContext(obj);
    QQmlContext *context = new QQmlContext(creationContext);
    context->setContextObject(obj);
    this->itemContextMap.insert(component, context);
    return context;
}

void AppStartupComponent::setBinder(AppStartupComponent *binder)
{
    if (_binder == binder)
        return;

    _binder = binder;
}

AppStartupComponent *AppStartupComponent::binder() const
{
    return _binder;
}

void AppStartupComponent::initRootItem(QQuickItem *item)
{
    QQuickItemPrivate *wp = QQuickItemPrivate::get(item);
    wp->addItemChangeListener(this, changedTypes);
}

void AppStartupComponent::deinitRootInit(QQuickItem *item)
{
    QQuickItemPrivate *wp = QQuickItemPrivate::get(item);
    wp->removeItemChangeListener(this, changedTypes);
}

void AppStartupComponent::copyTransitionGroupFromBinder()
{
    transitionGroup = binder() ? binder()->transitionGroup : nullptr;
}

QQmlContext *AppStartupComponent::transitionGroupContextFromBinder()
{
    AppStartupPreloadAttached *attached = qobject_cast<AppStartupPreloadAttached*>(
        qmlAttachedPropertiesObject<AppStartupPreloadAttached>(dd->appWindow, false));
    if (!attached)
        return nullptr;

    QQmlComponent *tgComponent = attached->transitionGroup();
    if (!tgComponent)
        return nullptr;

    if (binder()) {
        return binder()->itemContextMap.value(tgComponent);
    } else {
        return itemContextMap.value(tgComponent);;
    }
}

void AppStartupComponent::initialItemProperties(QQuickItem *item, AppStartupInitialProperties *initialProperties)
{
    if (!transitionGroup || !initialProperties)
        return;

    QHash<QString, int> rootPropertyHash;
    const QMetaObject *itemMetaObject = item->metaObject();
    for (int index = 0; index < itemMetaObject->propertyCount(); ++index) {
        const QMetaProperty &mtProp = itemMetaObject->property(index);
        rootPropertyHash.insert(QString::fromLatin1(mtProp.name()), index);
    }

    const QMetaObject *initMetaObject = initialProperties->metaObject();
    for (int index = initMetaObject->propertyOffset(); index < initMetaObject->propertyCount(); ++index) {
        const QMetaProperty &mtProp = initMetaObject->property(index);
        const QString &dynamicProperty = QString::fromLatin1(mtProp.name());
        if (rootPropertyHash.contains(dynamicProperty)) {
            if (mtProp.isBindable() && mtProp.bindable(item).hasBinding()) {
                auto binding = mtProp.bindable(item).takeBinding();
                qWarning() << "The item [" << item << "] has bind the property ["
                           << dynamicProperty << "], but initial propeties needed it, removed!";
            }

            QQmlProperty prop(item, dynamicProperty, qmlEngine(item));
            QQmlPropertyPrivate *privProp = QQmlPropertyPrivate::get(prop);
            const bool isValid = prop.isValid();
            if (isValid) {
                privProp->writeValueProperty(mtProp.read(initialProperties), {});
            }
        } else if (!dynamicProperty.startsWith(QLatin1String("_private"))
                   && !dynamicProperty.startsWith(QLatin1String("target"))) {
            qWarning() << "Dont find the propert: [" << dynamicProperty << "], from the target: " << item;
        }
    }

    //! ##TODO: support the vme meta properties.
}

void AppStartupComponent::transitionFinishedImpl()
{
    transitionFinish();

    if (AppStartupComponent *linkTo = transitionLinkNext())
        linkTo->startTransition(TrasitionBeginMode::BeginCurrent);

    if (transitionManager) {
        delete transitionManager;
        transitionManager = nullptr;
    }

    duringTransition = false;
}

bool AppStartupComponent::startTransition(TrasitionBeginMode mode)
{
    duringTransition = true;
    AppStartupComponent *headComponent = this;

    if (mode == TrasitionBeginMode::BeginHead) {
        while (headComponent->transitionLinkPrev()) {
            headComponent = headComponent->transitionLinkPrev();
        }

        if (headComponent != this) {
            return headComponent->startTransition(TrasitionBeginMode::BeginCurrent);
        }
    }

    QQuickItem *transitionItem = this->transitionItem();
    QQuickTransition *transition = this->transition();
    if (!transitionItem || !transition) {
        transitionFinishedImpl();
        return false;
    }

    transitionManager = new AppStartUpTransitionManager(nullptr);
    transitionManager->setFinishedCallback(std::bind(&AppStartupComponent::transitionFinishedImpl, this));
    beforeTransition();
    transitionManager->transition({}, transition, transitionItem);
    return true;
}
