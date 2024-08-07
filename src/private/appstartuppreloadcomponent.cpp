#include "appstartuppreloadcomponent.h"
#include "appstartupinstance_p.h"
#include "defines_p.h"

#include "interface/appstartuppreloadinterface.h"
#include "items/appstartupitem.h"
#include "items/appstartuptransitiongroup.h"

#include <QQuickItem>
#include <QQmlComponent>
#include <QQuickWindow>
#include <QQmlContext>
#include <QQmlApplicationEngine>
#include <QPluginLoader>

#include "private/qquicktransition_p.h"

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
        QQmlContext *context = itemContextMap.take(item->loadingOverlay());
        delete context;
    }
}

void AppStartupPreloadComponent::_q_onOverlayExitWhenChanged(bool changed)
{
    if (changed) {
        startTransition();
    }
}

void AppStartupPreloadComponent::createWindow()
{
    AppPreloadItem *item = appPreloadItem();
    if (!item)
        return;

    AppPreloadItem::PreloadView previewType = item->preloadView();
    QQmlComponent *windowComponent = nullptr;
    if (previewType == AppPreloadItem::Window) {
        windowComponent = new QQmlComponent(dd->engine.get(), QUrl("qrc:/appstartup/qml/container/Window.qml"), QQmlComponent::PreferSynchronous);
    } else if (previewType == AppPreloadItem::ApplicationWindow) {
        windowComponent = new QQmlComponent(dd->engine.get(), QUrl("qrc:/appstartup/qml/container/ApplicationWindow.qml"), QQmlComponent::PreferSynchronous);
    } else if (previewType == AppPreloadItem::Custom) {
        windowComponent = item->customPreloadView();
    }

    Q_ASSERT_X(windowComponent, "AppPreloadItem", "Construct the window component failed!");
    dd->appWindow = qobject_cast<QQuickWindow *>(windowComponent->beginCreate(creationContext(windowComponent, item)));
    Q_ASSERT_X(dd->appWindow, "AppPreloadItem", qPrintable("Create window failed, error string: " + windowComponent->errorString()));
    initialItemProperties(dd->appWindow, item->initialProperties());
    item->setWindow(dd->appWindow);
    windowComponent->completeCreate();

    if (dd->appWindow->isVisible()) {
        _q_onWindowVisibleChanged(true);
    } else {
        QObject::connect(dd->appWindow, &QQuickWindow::visibleChanged, this, &AppStartupPreloadComponent::_q_onWindowVisibleChanged, Qt::SingleShotConnection);
    }
}

void AppStartupPreloadComponent::_q_onWindowVisibleChanged(bool visible)
{
    if (visible) {
        preloadInstance->preloadCreated(dd->engine.get());
    }
}

void AppStartupPreloadComponent::itemGeometryChanged(QQuickItem *item, QQuickGeometryChange change, const QRectF &oldGeometry)
{
    if (loadingOverlay && !duringTransition) {
        if (overlayUsingParentSize)
            loadingOverlay->setSize(item->size());
    }
    QQuickItemChangeListener::itemGeometryChanged(item, change, oldGeometry);
}

AppStartupPreloadComponent::~AppStartupPreloadComponent()
{
    if (dd->windowContentItem)
        deinitRootInit(dd->windowContentItem);
}

QQuickTransition *AppStartupPreloadComponent::transition()
{
    return transitionGroup ? transitionGroup->leave() : nullptr;
}

AppStartupComponent *AppStartupPreloadComponent::transitionLinkNext()
{
    return binder();
}

void AppStartupPreloadComponent::transitionFinish()
{
    if (transitionGroup && !initialPropertiesHash.isEmpty()) {
        initialItemProperties(loadingOverlay, initialPropertiesHash);
        initialPropertiesHash.clear();
    }

    clearOverlay();

    if (dd->defaultAppStartItem) {
        dd->defaultAppStartItem->setEnabled(true);
        dd->defaultAppStartItem->setVisible(true);
    }
}

void AppStartupPreloadComponent::beforeTransition()
{
    if (transitionGroup)
        initialPropertiesHash = initialItemProperties(loadingOverlay, transitionGroup->leaveInitialProperties());
}

bool AppStartupPreloadComponent::load()
{
    QPluginLoader loader(this->information.path());
    preloadInstance = qobject_cast<AppStartupPreloadInterface *>(loader.instance());
    if (!preloadInstance) {
        //! @todo add error
        qFatal("load preload plugin failed!");
        return true;
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
        QCoreApplication::exit(-1);
        return;
    }

    setContentItem(qmlobject_cast<AppPreloadItem *>(obj));
    Q_ASSERT_X(contentItem(), "AppPreloadItem", "Preload root item only use the AppPreloadItem item!");

    createWindow();
    findWindowContentItem();
    Q_ASSERT(dd->windowContentItem);
    initRootItem(dd->windowContentItem);
    createOverlay();

    dd->loadEntityPlugins();
}

void AppStartupPreloadComponent::findWindowContentItem()
{
    if (!dd->appWindow)
        return;

    do {
        QVariant dataVariant = dd->appWindow->property(APPLICATIONWINDOW_CONTENTDATA);
        if (dataVariant.isValid()) {
            // ApplicationWindow
            auto objectsData = dataVariant.value<QQmlListProperty<QObject>>();
            dd->windowContentItem = qmlobject_cast<QQuickItem *>(objectsData.object);
            break;
        }
    } while (false);

    // Window
    if (!dd->windowContentItem)
        dd->windowContentItem = dd->appWindow->contentItem();
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
        tgContext = creationContext(tgComponent, dd->windowContentItem);
        transitionGroup = qobject_cast<AppStartupTransitionGroup *>(tgComponent->beginCreate(tgContext));
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

    QQmlContext *context = creationContext(loComponent, dd->windowContentItem);
    loadingOverlay = qobject_cast<QQuickItem *>(loComponent->beginCreate(context));
    if (!loadingOverlay) {
        loComponent->completeCreate();
        qWarning() << "Create loading overlay failed, error string: " << qPrintable(loComponent->errorString());
        return;
    }

    loadingOverlay->setParentItem(dd->windowContentItem);
    loadingOverlay->setZ(100);

    if (tgContext)
        tgContext->setContextProperty("leaveTarget", loadingOverlay);

    loComponent->completeCreate();

    QQuickItemPrivate *overlayPrivate = QQuickItemPrivate::get(loadingOverlay);
    if (!widthValid(overlayPrivate) || !heightValid(overlayPrivate)) {
        overlayUsingParentSize = true;
        if (qFuzzyCompare(dd->windowContentItem->width(), 0) || qFuzzyCompare(dd->windowContentItem->height(), 0)) {
            loadingOverlay->setSize(dd->appWindow->size());
        } else {
            loadingOverlay->setSize(dd->windowContentItem->size());
        }
    }

    dd->engine->setObjectOwnership(loadingOverlay, QQmlEngine::CppOwnership);
    return;
}
