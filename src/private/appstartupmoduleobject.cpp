#include "appstartupmoduleobject.h"
#include "appstartupcomponent.h"
#include "appstartupinstance_p.h"
#include "appstartuptransitionmanager.h"

#include "items/appstartupitem.h"
#include "items/appstartupinitialproperties.h"
#include "items/appstartuptransitiongroup.h"
#include "items/appstartuppreloaditem.h"

#include <private/qquickitem_p.h>

static const QQuickItemPrivate::ChangeTypes changedTypes = QQuickItemPrivate::Geometry;

AppStartupModuleObject::AppStartupModuleObject(const AppStartupModuleInformation &module, AppStartupInstancePrivate *dd)
    : qq(dd->qq)
    , dd(dd)
    , _information(module)
    , _duringTransition(false)
{

}

AppStartupModuleObject::~AppStartupModuleObject()
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

        if (AppStartupModuleObject *binder = this->binder())
            binder->deinitRootInit(_containerContentItem);

        _containerContentItem.clear();
    }

    if (_appSurfaceIsWindow && _surfacePointer.appSurfaceWindow) {
        _surfacePointer.appSurfaceWindow->close();
        _surfacePointer.appSurfaceWindow->destroy();
        _surfacePointer.appSurfaceWindow->deleteLater();
        _surfacePointer.appSurfaceWindow = nullptr;
    }

    unloadModule();
}

QQmlContext *AppStartupModuleObject::creationContext(QQmlComponent *component, QObject *obj)
{
    QQmlContext *creationContext = component->creationContext();
    if (!creationContext)
        creationContext = qmlContext(obj);
    QQmlContext *context = new QQmlContext(creationContext);
    context->setContextObject(obj);
    this->_itemContextMap.insert(component, context);
    return context;
}

void AppStartupModuleObject::setBinder(AppStartupModuleObject *binder)
{
    if (_binder == binder)
        return;

    _binder = binder;
}

AppStartupModuleObject *AppStartupModuleObject::binder() const
{
    return _binder;
}

QPointer<QQuickItem> AppStartupModuleObject::contentItem() const
{
    return _contentItem;
}

void AppStartupModuleObject::setContentItem(QQuickItem *item)
{
    if (_contentItem == item)
        return;

    _contentItem = item;
}

AppStartupModuleInformation AppStartupModuleObject::information() const
{
    return _information;
}

QSharedPointer<AppStartupModuleGroup> AppStartupModuleObject::group() const
{
    return _group;
}

void AppStartupModuleObject::setGroup(const QSharedPointer<AppStartupModuleGroup> &group)
{
    _group = group;
}

QVariant AppStartupModuleObject::moduleBindingProperty(AppStartupModuleGroup::BindingProperty type)
{
    if (!_group || !_group->isValid())
        return QVariant();

    return _group->bindingProperty(type);
}

void AppStartupModuleObject::insertModuleBindingProperty(AppStartupModuleGroup::BindingProperty type, const QVariant &value)
{
    if (!_group || !_group->isValid())
        return;

    _group->insertBindingProperty(type, value);
}

void AppStartupModuleObject::insertModuleBindingPropertyList(AppStartupModuleGroup::BindingProperty type, const QVariantList &value)
{
    if (!_group || !_group->isValid())
        return;

    _group->insertBindingPropertyList(type, value);
}

void AppStartupModuleObject::initRootItem(QQuickItem *item)
{
    QQuickItemPrivate *wp = QQuickItemPrivate::get(item);
    wp->addItemChangeListener(this, changedTypes);
}

void AppStartupModuleObject::deinitRootInit(QQuickItem *item)
{
    QQuickItemPrivate *wp = QQuickItemPrivate::get(item);
    wp->removeItemChangeListener(this, changedTypes);
}

void AppStartupModuleObject::copyTransitionGroupFromBinder()
{
    _transitionGroup = binder() ? binder()->_transitionGroup : nullptr;
}

QQmlContext *AppStartupModuleObject::transitionGroupContextFromBinder() const
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

QQuickWindow *AppStartupModuleObject::appWindowFromBinder() const
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

QQuickItem *AppStartupModuleObject::containerContentItemFromBinder()
{
    if (!binder())
        return nullptr;

    QQuickItem *contentItem = qmlobject_cast<QQuickItem *>(binder()->_containerContentItem);
    if (!contentItem)
        return nullptr;

    return contentItem;
}

QVariantHash AppStartupModuleObject::initialItemProperties(QObject *target, AppStartupInitialProperties *initialProperties)
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

QVariantHash AppStartupModuleObject::initialItemProperties(QObject *obj, const QVariantHash &properties)
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

QObject *AppStartupModuleObject::loadModule(const QString &path)
{
    if (!_loader)
        _loader = new QPluginLoader(path, this);

    if (_loader->isLoaded())
        return _loader->instance();

    if (_loader->load())
        return _loader->instance();

    return nullptr;
}

bool AppStartupModuleObject::unloadModule()
{
    if (!_loader)
        return true;

    if (!_loader->isLoaded())
        return true;

    bool unloaded = _loader->unload();
    qInfo() << "Unload the module: " << dd->appId << ", from path: "
            << _loader->fileName() << ", success: " << unloaded;
    return unloaded;
}

void AppStartupModuleObject::transitionFinishedImpl()
{
    transitionFinish();

    if (AppStartupModuleObject *linkTo = transitionLinkNext())
        linkTo->startTransition(TrasitionBeginMode::BeginCurrent);

    if (_transitionManager) {
        delete _transitionManager;
        _transitionManager = nullptr;
    }

    _duringTransition = false;
}

bool AppStartupModuleObject::startTransition(TrasitionBeginMode mode)
{
    _duringTransition = true;
    AppStartupModuleObject *headModule = this;

    if (mode == TrasitionBeginMode::BeginHead) {
        while (headModule->transitionLinkPrev()) {
            headModule = headModule->transitionLinkPrev();
        }

        if (headModule != this) {
            return headModule->startTransition(TrasitionBeginMode::BeginCurrent);
        }
    }

    QQuickItem *transitionItem = this->transitionItem();
    QQuickTransition *transition = this->transition();
    if (!transitionItem || !transition) {
        transitionFinishedImpl();
        return false;
    }

    _transitionManager = new AppStartUpTransitionManager(nullptr);
    _transitionManager->setFinishedCallback(std::bind(&AppStartupModuleObject::transitionFinishedImpl, this));
    beforeTransition();
    _transitionManager->transition({}, transition, transitionItem);
    return true;
}
