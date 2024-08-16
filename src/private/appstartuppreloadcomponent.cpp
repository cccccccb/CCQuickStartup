#include "appstartuppreloadcomponent.h"
#include "appstartupinstance_p.h"
#include "defines_p.h"

#include "interface/appstartuppreloadinterface.h"
#include "items/appstartupitem.h"
#include "items/appstartupitemattached.h"
#include "items/appstartuptransitiongroup.h"

#include <QQuickItem>
#include <QQmlComponent>
#include <QQuickWindow>
#include <QQmlContext>
#include <QQmlApplicationEngine>
#include <QPluginLoader>

#include <private/qquicktransition_p.h>

void AppStartupPreloadComponent::doOverlayAutoExitChanged(AppPreloadItem *attached)
{
    if (!attached->autoExitOverlay()) {
        QObject::connect(attached, &AppPreloadItem::overlayExitWhenChanged,
                         this, &AppStartupPreloadComponent::_q_onOverlayExitWhenChanged, Qt::SingleShotConnection);
        if (attached->overlayExitWhen()) {
            _q_onOverlayExitWhenChanged(true);
        }
    }
}

void AppStartupPreloadComponent::findContainerItem()
{
    if (_appSurfaceIsWindow) {
        findWindowContentItem();
    } else {
        _containerContentItem = _surfacePointer.appSurfaceItem;
    }
}

void AppStartupPreloadComponent::findWindowContentItem()
{
    if (!_surfacePointer.appSurfaceWindow)
        return;

    do {
        QVariant dataVariant = _surfacePointer.appSurfaceWindow->property(APPLICATIONWINDOW_CONTENTDATA);
        if (dataVariant.isValid()) {
            // ApplicationWindow
            auto objectsData = dataVariant.value<QQmlListProperty<QObject>>();
            _containerContentItem = qmlobject_cast<QQuickItem *>(objectsData.object);
            break;
        }
    } while (false);

    // Window
    if (!_containerContentItem)
        _containerContentItem = _surfacePointer.appSurfaceWindow->contentItem();

    //! @todo add custom surface property supported.
}

AppPreloadItem *AppStartupPreloadComponent::appPreloadItem() const
{
    if (contentItem().isNull())
        return nullptr;

    return qmlobject_cast<AppPreloadItem *>(contentItem());
}

void AppStartupPreloadComponent::clearOverlay()
{
    if (!loadingOverlay)
        return;

    loadingOverlay->setParentItem(nullptr);
    loadingOverlay->setVisible(false);
    loadingOverlay->deleteLater();
    loadingOverlay = nullptr;
    if (AppPreloadItem *item = appPreloadItem()) {
        QQmlContext *context = _itemContextMap.take(item->loadingOverlay());
        delete context;
    }
}

void AppStartupPreloadComponent::_q_onOverlayExitWhenChanged(bool changed)
{
    if (changed) {
        startTransition();
    }
}

bool AppStartupPreloadComponent::createSurface()
{
    AppPreloadItem *item = appPreloadItem();
    if (!item)
        return false;

    AppPreloadItem::PreloadSurface previewType = item->preloadSurface();
    QQmlComponent *surfaceComponent = nullptr;
    if (previewType == AppPreloadItem::WindowSurface) {
        surfaceComponent = new QQmlComponent(dd->engine.get(), QUrl("qrc:/appstartup/private/qml/container/Window.qml"), QQmlComponent::PreferSynchronous);
    } else if (previewType == AppPreloadItem::ApplicationWindowSurface) {
        surfaceComponent = new QQmlComponent(dd->engine.get(), QUrl("qrc:/appstartup/private/qml/container/ApplicationWindow.qml"), QQmlComponent::PreferSynchronous);
    } if (previewType == AppPreloadItem::CustomSurface) {
        surfaceComponent = item->customPreloadSurface();
    }

    if (surfaceComponent) {
        QObject *obj = surfaceComponent->beginCreate(creationContext(surfaceComponent, item));
        QQuickWindow *window = qobject_cast<QQuickWindow *>(obj);
        if (!window && dd->defaultComponentGroup == group()) {
            qFatal() << "Default component surface is not Window, error: " << surfaceComponent->errorString();
        } else if (window) {
            _appSurfaceIsWindow = true;
            _surfacePointer.appSurfaceWindow = window;
        } else if (QQuickItem *item = qobject_cast<QQuickItem *>(obj)) {
            _appSurfaceIsWindow = false;
            _surfacePointer.appSurfaceItem = item;
        } else {
            qWarning() << "Could not create the surface component, error: " << surfaceComponent->errorString();
            return false;
        }
    } else {
        auto defaultEntityComponent = dd->componentPluginHash.value(dd->defaultComponentGroup.entity());
        AppStartupItemAttached *defaultEntityAttached = qobject_cast<AppStartupItemAttached*>(
            qmlAttachedPropertiesObject<AppStartupItem>(defaultEntityComponent->contentItem(), false));
        if (!defaultEntityAttached) {
            qWarning() << "Could not find the default entity attached!";
            return false;
        }

        QQuickItem *container = defaultEntityAttached->componentContainer(this->group());
        if (!container) {
            qWarning() << "Could not find the item component container";
            return false;
        }

        _appSurfaceIsWindow = false;
        _surfacePointer.appSurfaceItem = item;
    }

    QObject *target = _appSurfaceIsWindow ? qobject_cast<QObject *>(_surfacePointer.appSurfaceWindow) : qobject_cast<QObject *>(_surfacePointer.appSurfaceItem);
    initialItemProperties(target, item->initialProperties());

    if (_appSurfaceIsWindow)
        item->setWindow(_surfacePointer.appSurfaceWindow);

    if (surfaceComponent)
        surfaceComponent->completeCreate();

    if (item->isVisible()) {
        _q_onPreloadItemVisibleChanged();
    } else {
        QObject::connect(item, &QQuickItem::visibleChanged, this, &AppStartupPreloadComponent::_q_onPreloadItemVisibleChanged, Qt::SingleShotConnection);
    }

    return true;
}

void AppStartupPreloadComponent::_q_onPreloadItemVisibleChanged()
{
    if (QQuickItem *item = qobject_cast<QQuickItem *>(sender())) {
        if (item->isVisible())
            preloadInstance->preloadCreated(dd->engine.get());
    }
}

void AppStartupPreloadComponent::itemGeometryChanged(QQuickItem *item, QQuickGeometryChange change, const QRectF &oldGeometry)
{
    if (loadingOverlay && !_duringTransition) {
        if (overlayUsingParentSize)
            loadingOverlay->setSize(item->size());
    }
    QQuickItemChangeListener::itemGeometryChanged(item, change, oldGeometry);
}

AppStartupPreloadComponent::~AppStartupPreloadComponent()
{
    qDebug() << "App startup preload component destruction";
    preloadInstance = nullptr;
}

QQuickTransition *AppStartupPreloadComponent::transition()
{
    return _transitionGroup ? _transitionGroup->leave() : nullptr;
}

AppStartupComponent *AppStartupPreloadComponent::transitionLinkNext()
{
    return binder();
}

void AppStartupPreloadComponent::transitionFinish()
{
    if (_transitionGroup && !initialPropertiesHash.isEmpty()) {
        initialItemProperties(loadingOverlay, initialPropertiesHash);
        initialPropertiesHash.clear();
    }

    clearOverlay();

    if (QQuickItem *item = binder()->contentItem()) {
        item->setEnabled(true);
        item->setVisible(true);
    }
}

void AppStartupPreloadComponent::beforeTransition()
{
    if (_transitionGroup)
        initialPropertiesHash = initialItemProperties(loadingOverlay, _transitionGroup->leaveInitialProperties());
}

bool AppStartupPreloadComponent::load()
{
    QObject *obj = this->loadPlugin(this->_information.path());
    if (!obj) {
        qFatal("Load the preload plugin failed!");
        return false;
    }

    preloadInstance = qobject_cast<AppStartupPreloadInterface *>(obj);
    if (!preloadInstance) {
        //! @todo add error
        qFatal("Convert the preload plugin failed!");
        return false;
    }

    preloadInstance->aboutToPreload(dd->engine.get());
    QObject::connect(dd->engine.get(), &QQmlApplicationEngine::objectCreated,
                     this, &AppStartupPreloadComponent::_q_onPreloadCreated);
    dd->engine->load(preloadInstance->preloadComponentPath());
    if (dd->engine->rootObjects().isEmpty()) {
        qWarning() << "No root object created!";
        return false;
    }

    return true;
}

void AppStartupPreloadComponent::_q_onPreloadCreated(QObject *obj, const QUrl &objUrl)
{
    if (!obj && preloadInstance->preloadComponentPath() == objUrl) {
        qWarning() << "Create component preload failed!";
        return;
    }

    setContentItem(qmlobject_cast<AppPreloadItem *>(obj));
    Q_ASSERT_X(contentItem(), "AppPreloadItem", "Preload root item only use the AppPreloadItem item!");

    if (!createSurface()) {
        qWarning() << "Create component preload surface failed!";
        return;
    }

    findContainerItem();
    Q_ASSERT(_containerContentItem);

    initRootItem(_containerContentItem);
    appPreloadItem()->setParentItem(_containerContentItem);
    createOverlay();

    dd->loadEntityPlugins(group());
}

void AppStartupPreloadComponent::createOverlay()
{
    if (loadingOverlay)
        return;

    AppPreloadItem *preloadItem = appPreloadItem();
    if (!preloadItem)
        return;

    QQmlComponent *tgComponent = preloadItem->transitionGroup();
    QQmlContext *tgContext = nullptr;
    if (tgComponent) {
        tgContext = creationContext(tgComponent, _containerContentItem);
        _transitionGroup = qobject_cast<AppStartupTransitionGroup *>(tgComponent->beginCreate(tgContext));
    }

    if (tgContext) {
        tgContext->setContextProperty("enterTarget", preloadItem->startupItem());
        tgContext->setContextProperty("leaveTarget", nullptr);
    } else {
        qWarning() << "No transition group or type is not the AppStartupTransitionGroup!";
    }

    if (tgComponent)
        tgComponent->completeCreate();

    QQmlComponent *loComponent = preloadItem->loadingOverlay();
    if (!loComponent)
        return;

    QObject::connect(preloadItem, &AppPreloadItem::autoExitOverlayChanged, this,
                     std::bind(&AppStartupPreloadComponent::doOverlayAutoExitChanged, this, preloadItem));
    doOverlayAutoExitChanged(preloadItem);

    QQmlContext *context = creationContext(loComponent, _containerContentItem);
    loadingOverlay = qobject_cast<QQuickItem *>(loComponent->beginCreate(context));
    if (!loadingOverlay) {
        loComponent->completeCreate();
        qWarning() << "Create loading overlay failed, error string: " << qPrintable(loComponent->errorString());
        return;
    }

    loadingOverlay->setParentItem(_containerContentItem);
    loadingOverlay->setZ(100);

    if (tgContext)
        tgContext->setContextProperty("leaveTarget", loadingOverlay);

    loComponent->completeCreate();

    QQuickItemPrivate *overlayPrivate = QQuickItemPrivate::get(loadingOverlay);
    if (!widthValid(overlayPrivate) || !heightValid(overlayPrivate)) {
        overlayUsingParentSize = true;
        loadingOverlay->setSize(_containerContentItem->size());
    }

    dd->engine->setObjectOwnership(loadingOverlay, QQmlEngine::CppOwnership);
    return;
}
