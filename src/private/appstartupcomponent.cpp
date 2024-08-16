#include "appstartupcomponent.h"
#include "appstartupinstance_p.h"
#include "appstartuptransitionmanager.h"
#include "defines_p.h"

#include "items/appstartupitem.h"
#include "items/appstartupinitialproperties.h"
#include "items/appstartuptransitiongroup.h"
#include "items/appstartuppreloaditem.h"

#include <private/qquickitem_p.h>

static const QQuickItemPrivate::ChangeTypes changedTypes = QQuickItemPrivate::Geometry;

AppStartupComponent::AppStartupComponent(const AppStartupComponentInformation &plugin, AppStartupInstancePrivate *dd)
    : qq(dd->qq)
    , dd(dd)
    , _information(plugin)
    , _duringTransition(false)
{

}

AppStartupComponent::~AppStartupComponent()
{
    qDeleteAll(_itemContextMap);
    _itemContextMap.clear();

    if (_transitionManager) {
        if (_transitionManager->isRunning())
            _transitionManager->cancel();

        delete _transitionManager;
        _transitionManager = nullptr;
    }

    if (_containerContentItem) {
        deinitRootInit(_containerContentItem);

        if (AppStartupComponent *binder = this->binder())
            binder->deinitRootInit(_containerContentItem);

        _containerContentItem.clear();
    }

    if (_appSurfaceIsWindow && _surfacePointer.appSurfaceWindow) {
        _surfacePointer.appSurfaceWindow->close();
        _surfacePointer.appSurfaceWindow->destroy();
        _surfacePointer.appSurfaceWindow->deleteLater();
        _surfacePointer.appSurfaceWindow = nullptr;
    }

    unloadPlugin();
}

QQmlContext *AppStartupComponent::creationContext(QQmlComponent *component, QObject *obj)
{
    QQmlContext *creationContext = component->creationContext();
    if (!creationContext)
        creationContext = qmlContext(obj);
    QQmlContext *context = new QQmlContext(creationContext);
    context->setContextObject(obj);
    this->_itemContextMap.insert(component, context);
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

QPointer<QQuickItem> AppStartupComponent::contentItem() const
{
    return _contentItem;
}

void AppStartupComponent::setContentItem(QQuickItem *item)
{
    if (_contentItem == item)
        return;

    _contentItem = item;
}

AppStartupComponentInformation AppStartupComponent::information() const
{
    return _information;
}

AppStartupComponentGroup AppStartupComponent::group() const
{
    return _group;
}

void AppStartupComponent::setGroup(const AppStartupComponentGroup &group)
{
    _group = group;
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
    _transitionGroup = binder() ? binder()->_transitionGroup : nullptr;
}

QQmlContext *AppStartupComponent::transitionGroupContextFromBinder() const
{
    if (!binder())
        return nullptr;

    AppPreloadItem *preloadItem = qmlobject_cast<AppPreloadItem *>(binder()->contentItem());
    if (!preloadItem)
        return nullptr;

    QQmlComponent *tgComponent = preloadItem->transitionGroup();
    if (!tgComponent)
        return nullptr;

    if (binder()) {
        return binder()->_itemContextMap.value(tgComponent);
    }

    return _itemContextMap.value(tgComponent);
}

QQuickWindow *AppStartupComponent::appWindowFromBinder() const
{
    if (!binder())
        return nullptr;

    if (!binder()->_appSurfaceIsWindow)
        return nullptr;

    QQuickWindow *window = binder()->_surfacePointer.appSurfaceWindow;
    if (!window)
        return nullptr;

    return window;
}

QQuickItem *AppStartupComponent::containerContentItemFromBinder()
{
    if (!binder())
        return nullptr;

    QQuickItem *contentItem = qmlobject_cast<QQuickItem *>(binder()->_containerContentItem);
    if (!contentItem)
        return nullptr;

    return contentItem;
}

QVariantHash AppStartupComponent::initialItemProperties(QObject *target, AppStartupInitialProperties *initialProperties)
{
    if (!initialProperties)
        return {};

    QVariantHash rootPropertyHash;
    const QMetaObject *initMetaObject = initialProperties->metaObject();
    for (int index = initMetaObject->propertyOffset(); index < initMetaObject->propertyCount(); ++index) {
        const QMetaProperty &mtProp = initMetaObject->property(index);
        const QString &dynamicProperty = QString::fromLatin1(mtProp.name());
        if (!dynamicProperty.startsWith(QLatin1String("_private"))
            && !dynamicProperty.startsWith(QLatin1String("target"))) {
            const QMetaProperty &mtProp = initMetaObject->property(index);
            rootPropertyHash.insert(QString::fromLatin1(mtProp.name()), mtProp.read(initialProperties));
        }
    }

    QVariantHash prevPropertiesHash = initialItemProperties(target, rootPropertyHash);
    return prevPropertiesHash;
}

QVariantHash AppStartupComponent::initialItemProperties(QObject *obj, const QVariantHash &properties)
{
    QVariantHash prevPropertiesHash;
    QVariantHash applyPropertiesHash = properties;

    const QMetaObject *itemMetaObject = obj->metaObject();
    for (int index = 0; index < itemMetaObject->propertyCount(); ++index) {
        const QMetaProperty &mtProp = itemMetaObject->property(index);
        const QString &dynamicProperty = QString::fromLatin1(mtProp.name());

        auto it = applyPropertiesHash.find(dynamicProperty);
        if (it != applyPropertiesHash.end()) {
            // if (mtProp.isBindable() && mtProp.bindable(obj).hasBinding()) {
            //     auto binding = mtProp.bindable(item).takeBinding();
            //     qWarning() << "The item [" << item << "] has bind the property ["
            //                << dynamicProperty << "], but initial propeties needed it, removed!";
            // }

            QQmlProperty prop(obj, dynamicProperty, qmlEngine(obj));
            QQmlPropertyPrivate *privProp = QQmlPropertyPrivate::get(prop);
            const bool isValid = prop.isValid();
            if (isValid) {
                prevPropertiesHash.insert(dynamicProperty, privProp->readValueProperty());
                privProp->writeValueProperty(applyPropertiesHash.value(dynamicProperty), {});
            }

            applyPropertiesHash.erase(it);
        }
    }

    for (const auto &noExistProp : applyPropertiesHash.keys())
        qWarning() << "Dont find the propert: [" << noExistProp << "], from the target: " << obj;

    //! ##TODO: support the vme meta properties.

    return prevPropertiesHash;
}

QObject *AppStartupComponent::loadPlugin(const QString &path)
{
    if (!_loader)
        _loader = new QPluginLoader(path, this);

    if (_loader->isLoaded())
        return _loader->instance();

    if (_loader->load())
        return _loader->instance();

    return nullptr;
}

bool AppStartupComponent::unloadPlugin()
{
    if (!_loader)
        return true;

    if (!_loader->isLoaded())
        return true;

    bool unloaded = _loader->unload();
    qInfo() << "Unload the plugin: " << dd->appId << ", from path: "
            << _loader->fileName() << ", success: " << unloaded;
    return unloaded;
}

void AppStartupComponent::transitionFinishedImpl()
{
    transitionFinish();

    if (AppStartupComponent *linkTo = transitionLinkNext())
        linkTo->startTransition(TrasitionBeginMode::BeginCurrent);

    if (_transitionManager) {
        delete _transitionManager;
        _transitionManager = nullptr;
    }

    _duringTransition = false;
}

bool AppStartupComponent::startTransition(TrasitionBeginMode mode)
{
    _duringTransition = true;
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

    _transitionManager = new AppStartUpTransitionManager(nullptr);
    _transitionManager->setFinishedCallback(std::bind(&AppStartupComponent::transitionFinishedImpl, this));
    beforeTransition();
    _transitionManager->transition({}, transition, transitionItem);
    return true;
}
