#include "appstartuppreloadmoduleobject.h"
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
#include <QFileInfo>

#include <private/qquicktransition_p.h>

static constexpr qreal kLoadingOverlayZ = 99.0;

void AppStartupPreloadModuleObject::doOverlayAutoExitChanged(AppPreloadItem *attached)
{
    if (!attached->autoExitOverlay()) {
        QObject::connect(attached, &AppPreloadItem::overlayExitWhenChanged,
                         this, &AppStartupPreloadModuleObject::_q_onOverlayExitWhenChanged, Qt::SingleShotConnection);
        if (attached->overlayExitWhen()) {
            _q_onOverlayExitWhenChanged(true);
        }
    }
}

void AppStartupPreloadModuleObject::findContainerItem()
{
    if (_appSurfaceIsWindow) {
        findWindowContentItem();
    } else {
        _containerContentItem = _surfacePointer.appSurfaceItem;
    }
}

void AppStartupPreloadModuleObject::findWindowContentItem()
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
}

AppPreloadItem *AppStartupPreloadModuleObject::appPreloadItem() const
{
    if (contentItem().isNull())
        return nullptr;

    return qmlobject_cast<AppPreloadItem *>(contentItem());
}

void AppStartupPreloadModuleObject::clearOverlay()
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

void AppStartupPreloadModuleObject::_q_onOverlayExitWhenChanged(bool changed)
{
    if (changed) {
        startTransition();
    }
}

bool AppStartupPreloadModuleObject::createSurface()
{
    AppPreloadItem *item = appPreloadItem();
    if (!item)
        return false;

    AppPreloadItem::PreloadSurface previewType = item->preloadSurface();
    QQmlComponent *surfaceComponent = nullptr;
    if (previewType == AppPreloadItem::WindowSurface) {
        surfaceComponent = new QQmlComponent(dd->engine.get(), QUrl("qrc:/appstartup/private/qml/container/Window.qml"), QQmlComponent::PreferSynchronous, this);
    } else if (previewType == AppPreloadItem::ApplicationWindowSurface) {
        surfaceComponent = new QQmlComponent(dd->engine.get(), QUrl("qrc:/appstartup/private/qml/container/ApplicationWindow.qml"), QQmlComponent::PreferSynchronous, this);
    } else if (previewType == AppPreloadItem::CustomSurface) {
        surfaceComponent = item->customPreloadSurface();
    }

    if (surfaceComponent) {
        QObject *obj = surfaceComponent->beginCreate(creationContext(surfaceComponent, item));
        QQuickWindow *window = qobject_cast<QQuickWindow *>(obj);
        if (!window && dd->defaultModuleGroup == group()) {
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
        auto defaultEntityModule = dd->componentModuleHash.value(dd->defaultModuleGroup->entity());

        QQuickItem *container = qvariant_cast<QQuickItem *>(moduleBindingProperty(AppStartupModuleGroup::SurfaceItem));
        if (!container) {
            qWarning() << "Could not find the item module container";
            return false;
        }

        _appSurfaceIsWindow = false;
        _surfacePointer.appSurfaceItem = container;
    }

    QObject *target = _appSurfaceIsWindow ? qobject_cast<QObject *>(_surfacePointer.appSurfaceWindow) : qobject_cast<QObject *>(_surfacePointer.appSurfaceItem);
    initialItemProperties(target, item->initialProperties());

    if (_appSurfaceIsWindow)
        item->setWindow(_surfacePointer.appSurfaceWindow);

    if (surfaceComponent)
        surfaceComponent->completeCreate();

    if (_appSurfaceIsWindow) {
        if (item->isVisible()) {
            _q_onPreloadItemVisibleChanged();
        } else {
            QObject::connect(item, &QQuickItem::visibleChanged, this, &AppStartupPreloadModuleObject::_q_onPreloadItemVisibleChanged, Qt::SingleShotConnection);
        }
    } else if (item->isComponentComplete()) {
        _q_onPreloadItemVisibleChanged();
    }

    return true;
}

void AppStartupPreloadModuleObject::_q_onPreloadItemVisibleChanged()
{
    if (QQuickItem *item = qobject_cast<QQuickItem *>(sender())) {
        if (item->isVisible())
            preloadInstance->preloadCreated(dd->engine.get());
    }
}

void AppStartupPreloadModuleObject::itemGeometryChanged(QQuickItem *item, QQuickGeometryChange change, const QRectF &oldGeometry)
{
    if (loadingOverlay && !_duringTransition) {
        if (overlayUsingParentSize)
            loadingOverlay->setSize(item->size());
    }
    QQuickItemChangeListener::itemGeometryChanged(item, change, oldGeometry);
}

AppStartupPreloadModuleObject::~AppStartupPreloadModuleObject()
{
    qDebug() << "App startup preload module destruction";
    preloadInstance = nullptr;
    clearOverlay();
}

QQuickTransition *AppStartupPreloadModuleObject::transition()
{
    return _transitionGroup ? _transitionGroup->leave() : nullptr;
}

AppStartupModuleObject *AppStartupPreloadModuleObject::transitionLinkNext()
{
    return binder();
}

void AppStartupPreloadModuleObject::transitionFinish()
{
    if (_transitionGroup && !initialPropertiesHash.isEmpty()) {
        initialItemProperties(loadingOverlay, initialPropertiesHash);
        initialPropertiesHash.clear();
    }

    clearOverlay();

    AppStartupModuleObject *binder = this->binder();
    if (!binder)
        return;

    if (QQuickItem *item = binder->contentItem()) {
        item->setEnabled(true);
        item->setVisible(true);
    }
}

void AppStartupPreloadModuleObject::beforeTransition()
{
    if (_transitionGroup)
        initialPropertiesHash = initialItemProperties(loadingOverlay, _transitionGroup->leaveInitialProperties());
}

bool AppStartupPreloadModuleObject::load()
{
    QObject *obj = this->loadModule(this->_information.path());
    if (!obj) {
        const QString &errorString = "Load the preload module failed, " + this->_information.path();
        qWarning() << errorString;

        Q_EMIT qq->errorOccured(group(), errorString);
        return false;
    }

    preloadInstance = qobject_cast<AppStartupPreloadInterface *>(obj);
    if (!preloadInstance) {
        const QString &errorString = "Convert the preload module failed, " + this->_information.path();
        qWarning() << errorString;

        Q_EMIT qq->errorOccured(group(), errorString);
        return false;
    }

    preloadInstance->aboutToPreload(dd->engine.get());
    QObject::connect(dd->engine.get(), &QQmlApplicationEngine::objectCreated,
                     this, &AppStartupPreloadModuleObject::_q_onPreloadCreated, Qt::SingleShotConnection);
    dd->engine->load(preloadInstance->preloadModulePath());
    if (dd->engine->rootObjects().isEmpty()) {
        const QString &errorString = "No root object created!";
        qWarning() << errorString;

        Q_EMIT qq->errorOccured(group(), errorString);
        return false;
    }

    return true;
}

void AppStartupPreloadModuleObject::_q_onPreloadCreated(QObject *obj, const QUrl &objUrl)
{
    const QString objPath = objUrl.toString();
    const QString modulePath = preloadInstance->preloadModulePath().toString();
    if (objUrl.isLocalFile() && QFileInfo(objPath) != QFileInfo(modulePath)) {
        const QString &errorString = QString("Preload module path [%1] is invalid!").arg(objPath);
        qWarning() << errorString;

        Q_EMIT qq->errorOccured(group(), errorString);
        return;
    }

    if (objUrl != preloadInstance->preloadModulePath() && QFileInfo(objPath) != QFileInfo(modulePath)) {
        const QString &errorString = QString("Preload module path [%1] is invalid!").arg(objPath);
        qWarning() << errorString;

        Q_EMIT qq->errorOccured(group(), errorString);
        return;
    }

    if (!obj) {
        const QString &errorString = "Create module preload failed!";
        qWarning() << errorString;

        Q_EMIT qq->errorOccured(group(), errorString);
        return;
    }

    setContentItem(qmlobject_cast<AppPreloadItem *>(obj));
    if (!contentItem()) {
        qWarning() << "Preload root item only use the AppPreloadItem item!";
        return;
    }

    if (!createSurface()) {
        const QString &errorString = "Create module preload surface failed!";
        qWarning() << errorString;

        Q_EMIT qq->errorOccured(group(), errorString);
        return;
    }

    findContainerItem();
    if (!_containerContentItem) {
        qWarning() << "Could not find container content item";
        return;
    }

    initRootItem(_containerContentItem);
    appPreloadItem()->setParentItem(_containerContentItem);
    createOverlay();

    dd->loadEntityModules(group());
}

void AppStartupPreloadModuleObject::createOverlay()
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
                     std::bind(&AppStartupPreloadModuleObject::doOverlayAutoExitChanged, this, preloadItem));
    QMetaObject::invokeMethod(this, [this, preloadItem]() { doOverlayAutoExitChanged(preloadItem);}, Qt::QueuedConnection);

    QQmlContext *context = creationContext(loComponent, _containerContentItem);
    loadingOverlay = qobject_cast<QQuickItem *>(loComponent->beginCreate(context));
    if (!loadingOverlay) {
        loComponent->completeCreate();
        qWarning() << "Create loading overlay failed, error string: " << qPrintable(loComponent->errorString());
        return;
    }

    loadingOverlay->setParentItem(_containerContentItem);
    loadingOverlay->setZ(kLoadingOverlayZ);

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
