#include "appstartuppreloadcomponent.h"
#include "appstartupinstance_p.h"
#include "ccstartuppreloadinterface.h"
#include "defines_p.h"

#include "items/appstartuppreloadattached.h"
#include "items/appstartupitem.h"
#include "items/appstartuptransitiongroup.h"

#include <QQuickItem>
#include <QQmlComponent>
#include <QQuickWindow>
#include <QQmlContext>
#include <QQmlApplicationEngine>
#include <QPluginLoader>

#include "private/qquicktransition_p.h"

void AppStartupPreloadComponent::doOverlayAutoExitChanged(AppStartupPreloadAttached *attached)
{
    if (attached->autoExitOverlay()) {
        if (autoExitConnection)
            QObject::disconnect(autoExitConnection);
    } else {
        autoExitConnection = QObject::connect(attached, &AppStartupPreloadAttached::overlayExitWhenChanged,
                                              this, &AppStartupPreloadComponent::_q_onOverlayExitWhenChanged);
        if (attached->overlayExitWhen()) {
            _q_onOverlayExitWhenChanged(true);
        }
    }
}

void AppStartupPreloadComponent::clearOverlay()
{
    if (!loadingOverlay)
        return;

    loadingOverlay->setParentItem(nullptr);
    loadingOverlay->setVisible(false);
    loadingOverlay->deleteLater();
    loadingOverlay = nullptr;
    AppStartupPreloadAttached *attached = qobject_cast<AppStartupPreloadAttached*>(
        qmlAttachedPropertiesObject<AppStartupPreloadAttached>(dd->appWindow, false));
    if (attached) {
        QQmlContext *context = itemContextMap.take(attached->loadingOverlay());
        delete context;
    }
}

void AppStartupPreloadComponent::_q_onOverlayExitWhenChanged(bool changed)
{
    if (changed) {
        startTransition();
    }
}

void AppStartupPreloadComponent::_q_onWindowVisibleChanged(bool visible)
{
    if (visible) {
        preloadInstance->preloadCreated(dd->engine.get());
        if (visibleConnection)
            QObject::disconnect(visibleConnection);
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
    clearOverlay();

    if (dd->defaultAppStartItem) {
        dd->defaultAppStartItem->setEnabled(true);
        dd->defaultAppStartItem->setVisible(true);
    }
}

void AppStartupPreloadComponent::beforeTransition()
{
    if (transitionGroup)
        initialItemProperties(loadingOverlay, transitionGroup->leaveInitialProperties());
}

void AppStartupPreloadComponent::load()
{
    const AppStartupComponentInformation &pluginInfo = this->information;
    QPluginLoader loader(pluginInfo.path());
    preloadInstance = qobject_cast<CCStartupPreloadInterface *>(loader.instance());
    if (!preloadInstance) {
        //! @todo add error
        qFatal("load preload plugin failed!");
        return;
    }

    preloadInstance->aboutToPreload(dd->engine.get());
    QObject::connect(dd->engine.get(), &QQmlApplicationEngine::objectCreated,
                     this, &AppStartupPreloadComponent::_q_onPreloadCreated);
    dd->engine->load(preloadInstance->preloadComponentPath());
    if (dd->engine->rootObjects().isEmpty()) {
        qWarning() << "No root object created!";
        qGuiApp->exit(-1);
        return;
    }
}

void AppStartupPreloadComponent::_q_onPreloadCreated(QObject *obj, const QUrl &objUrl)
{
    if (!obj && preloadInstance->preloadComponentPath() == objUrl) {
        QCoreApplication::exit(-1);
        return;
    }

    QQuickWindow *window = qmlobject_cast<QQuickWindow *>(obj);
    if (!window) {
        QCoreApplication::exit(-1);
        return;
    }

    dd->appWindow = window;
    Q_ASSERT(dd->appWindow);

    if (dd->appWindow->isVisible()) {
        _q_onWindowVisibleChanged(true);
    } else {
        visibleConnection = QObject::connect(dd->appWindow, &QQuickWindow::visibleChanged,
                                             this, &AppStartupPreloadComponent::_q_onWindowVisibleChanged);
    }

    findWindowContentItem();
    Q_ASSERT(dd->windowContentItem);
    initRootItem(dd->windowContentItem);
    createOverlay();

    dd->loadMainWindowPlugins();
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

        dataVariant = dd->appWindow->property(DIALOGWINDOW_CONTENTDATA);
        if (dataVariant.isValid()) {
            // DialogWindow
            auto itemsData = dataVariant.value<QQmlListProperty<QQuickItem>>();
            dd->windowContentItem = qmlobject_cast<QQuickItem *>(itemsData.object);
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

    AppStartupPreloadAttached *attached = qobject_cast<AppStartupPreloadAttached*>(
        qmlAttachedPropertiesObject<AppStartupPreloadAttached>(dd->appWindow, false));
    if (!attached)
        return;

    QQmlComponent *tgComponent = attached->transitionGroup();
    QQmlContext *tgContext = nullptr;
    if (tgComponent) {
        tgContext = creationContext(tgComponent, dd->windowContentItem);
        transitionGroup = qobject_cast<AppStartupTransitionGroup *>(tgComponent->beginCreate(tgContext));
    }

    if (tgContext) {
        tgContext->setContextProperty("enterTarget", attached->startupItem());
        tgContext->setContextProperty("leaveTarget", nullptr);
    } else {
        qWarning() << "[transitionGroup] only use AppStartupTransitionGroup type!";
    }

    if (tgComponent)
        tgComponent->completeCreate();

    QQmlComponent *loComponent = attached->loadingOverlay();
    if (!loComponent)
        return;

    QObject::connect(attached, &AppStartupPreloadAttached::autoExitOverlayChanged, this,
                     std::bind(&AppStartupPreloadComponent::doOverlayAutoExitChanged, this, attached));
    doOverlayAutoExitChanged(attached);

    QQmlContext *context = creationContext(loComponent, dd->windowContentItem);
    loadingOverlay = qobject_cast<QQuickItem *>(loComponent->beginCreate(context));
    if (!loadingOverlay)
        return;

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
