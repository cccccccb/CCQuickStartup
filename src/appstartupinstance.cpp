#include <QPluginLoader>
#include <QQmlComponent>
#include <QScopedPointer>
#include <QUrl>
#include <QQuickWindow>
#include <QQuickItem>
#include <QQmlApplicationEngine>
#include <QQmlIncubator>
#include <QString>
#include <QQmlIncubationController>
#include <QDir>
#include <QQmlContext>
#include <QJsonArray>

#include <private/qsgrenderloop_p.h>
#include <private/qquickitemchangelistener_p.h>
#include <private/qquicktransitionmanager_p_p.h>
#include <private/qquickitem_p.h>
#include <private/qquicktransition_p.h>

#include "appstartupinstance.h"
#include "appstartuppreloadattached.h"
#include "appstartupinstanceattached.h"
#include "quickappstartupitem.h"
#include "ccquickstartuppreloadinterface.h"
#include "ccquickstartupmainwindowinterface.h"

#define APPLICATIONWINDOW_CONTENTDATA "contentData"
#define DIALOGWINDOW_CONTENTDATA "content"
#define WINDOW_CONTENTDATA "data"

static const QQuickItemPrivate::ChangeTypes changedTypes = QQuickItemPrivate::Geometry;
AppStartupInstance *AppStartupInstance::self = nullptr;

static inline bool heightValid(QQuickItemPrivate *item)
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    return item->heightValid;
#else
    return item->heightValid();
#endif
}
static inline bool widthValid(QQuickItemPrivate *item)
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    return item->widthValid;
#else
    return item->widthValid();
#endif
}

class AppStartupComponent;
class AppStartUpTransitionManager : public QQuickTransitionManager
{
public:
    explicit AppStartUpTransitionManager(const std::function<void()> &finishedCallback)
        : QQuickTransitionManager()
        , _finishedCallback(finishedCallback)
    { }

    void setFinishedCallback(const std::function<void()> &finishedCallback) {
        _finishedCallback = finishedCallback;
    }

protected:
    void finished() override {
        if (_finishedCallback)
            _finishedCallback();
    }

private:
    std::function<void()> _finishedCallback;
};


QT_BEGIN_NAMESPACE
class QQmlApplicationEngine;
class QQuickItem;
class QQuickTransition;
QT_END_NAMESPACE


class QuickAppStartupItem;
class CCQuickStartupPreloadInterface;
class CCQuickStartupMainWindowInterface;
class AppStartupComponent : public QQuickItemChangeListener
{
    friend class AppQmlComponentIncubator;
public:
    AppStartupComponent(const AppStartupComponentInformation &plugin, AppStartupInstancePrivate *dd);
    ~AppStartupComponent();

    void setBinder(AppStartupComponent *binder);
    AppStartupComponent *binder() const;

    virtual AppStartupComponentInformation::StartComponent componentType() = 0;
    virtual void load() {}

    virtual QQuickItem *transitionItem() { return nullptr; }
    virtual QQuickTransition *transition() { return nullptr; }
    virtual AppStartupComponent *transitionLinkNext() {return nullptr;}
    virtual AppStartupComponent *transitionLinkPrev() {return nullptr;}

    enum TrasitionBeginMode { BeginHead, BeginCurrent };
    virtual bool startTransition(TrasitionBeginMode mode = TrasitionBeginMode::BeginHead);
    virtual void transitionFinish() {}

protected:
    QQmlContext *creationContext(QQmlComponent *component, QObject *obj);
    void initRootItem(QQuickItem *item);

    AppStartupInstance *qq = nullptr;
    AppStartupInstancePrivate *dd = nullptr;

    AppStartupComponent *_binder = nullptr;
    AppStartupComponentInformation information;

    bool duringTransition = false;
    QHash<QQmlComponent *, QQmlContext *> itemContextMap;

private:
    void transitionFinishedImpl(AppStartUpTransitionManager *transitionManager);
};

class AppStartupPreloadComponent : public AppStartupComponent {
public:
    using AppStartupComponent::AppStartupComponent;

    inline AppStartupComponentInformation::StartComponent componentType() override
    {
        return AppStartupComponentInformation::Preload;
    };

    inline QQuickItem *transitionItem() override { return loadingOverlay; }
    virtual QQuickTransition *transition() override { return overlayExitTransition; }

    AppStartupComponent *transitionLinkNext() override;
    void transitionFinish() override;

    virtual void load() override;

protected:
    void itemGeometryChanged(QQuickItem *item, QQuickGeometryChange change, const QRectF &oldGeometry) override;

private:
    void _q_onPreloadCreated(QObject *obj, const QUrl &);
    void _q_onWindowVisibleChanged(bool visible);
    void _q_onOverlayExitWhenChanged(bool changed);

    void findWindowContentItem();
    void createOverlay();
    void clearOverlay();
    void doOverlayAutoExitChanged(AppStartupPreloadAttached *attached);

private:
    CCQuickStartupPreloadInterface *preloadInstance = nullptr;
    QQuickItem *loadingOverlay = nullptr;
    QMetaObject::Connection visibleConnection;
    QMetaObject::Connection autoExitConnection;
    QQuickTransition *overlayExitTransition = nullptr;
    bool overlayUsingParentSize = false;
};

class AppStartupMainWindowComponent : public AppStartupComponent
{
    friend class AppQmlComponentIncubator;
public:
    using AppStartupComponent::AppStartupComponent;

    inline AppStartupComponentInformation::StartComponent componentType() override
    {
        return AppStartupComponentInformation::MainWindow;
    };

    inline QQuickItem *transitionItem() override { return appRootItem; }
    inline QQuickTransition *transition() override { return rootDisplayTransition; }

    AppStartupComponent *transitionLinkPrev() override;
    void transitionFinish() override;

    void load() override;

protected:
    void itemGeometryChanged(QQuickItem *item, QQuickGeometryChange change, const QRectF &oldGeometry) override;

private:
    void _q_onMainComponentStatusChanged(QQmlComponent::Status status);
    void _q_onComponentProgressChanged();

    bool createObjects(const char *propertyName);
    void createChildComponents();
    void updateRootItemSize(QQuickItem *item);
    void destoryIncubator(QQmlIncubator *incubator);
    void finishedLoaded();
    void endOfTransition();

private:
    QScopedPointer<CCQuickStartupMainWindowInterface> mainInstance;
    QQmlComponent *mainComponent = nullptr;
    QuickAppStartupItem *appRootItem = nullptr;

    QQuickTransition *rootDisplayTransition = nullptr;
    QList<QQmlIncubator *> incubators;

    int childrenCount = 0;
};

class AppStartupComponentFactory
{
public:
    AppStartupComponentFactory(AppStartupInstancePrivate *dd)
        : dd(dd)
    {

    }

    inline AppStartupComponent *create(const AppStartupComponentInformation &plugin)
    {
        if (plugin.startComponent() == AppStartupComponentInformation::Preload) {
            return new AppStartupPreloadComponent(plugin, dd);
        } else if (plugin.startComponent() == AppStartupComponentInformation::MainWindow) {
            return new AppStartupMainWindowComponent(plugin, dd);
        }

        return nullptr;
    }

private:
    AppStartupInstancePrivate *dd;
};

class AppStartupInstancePrivate
{
    friend class AppQmlComponentIncubator;
    friend class AppStartupComponent;
public:
    AppStartupInstancePrivate(AppStartupInstance *qq);

    void scanPlugins();
    QList<AppStartupComponentInformation> scanDynamicPlugins();
    QList<AppStartupComponentInformation> scanStaticPlugins();
    void detachAvailablePluginsChange(const QList<AppStartupComponentInformation> &plugins);

    void reloadPlugins();
    void unloadPlugins();

    void findDefaultComponentGroup();
    void loadPreloadPlugins();
    void loadMainWindowPlugins();
    bool resolveMetaInfoFromObject(const QJsonObject &obj, AppStartupComponentInformation *info);

    static QStringList buildinPluginPaths();

    AppStartupInstance *qq = nullptr;
    QScopedPointer<QGuiApplication> app;
    QScopedPointer<QQmlApplicationEngine> engine;

    QString appId;

    QScopedPointer<AppStartupComponentFactory> componentFactory;
    QQuickWindow *appWindow = nullptr;
    QPointer<QQuickItem> windowContentItem;
    QPointer<QuickAppStartupItem> defaultAppStartItem;
    AppStartupComponentGroup defaultComponentGroup;

    QStringList pluginPaths;

    QList<AppStartupComponentGroup> availablePlugins;
    QList<AppStartupComponentGroup> reloadPluginsList;
    QList<AppStartupComponentGroup> loadedPluginsList;

    QHash<AppStartupComponentInformation, QSharedPointer<AppStartupComponent>> componentPluginHash;
};

class AppQmlComponentIncubator : public QQmlIncubator
{
public:
    AppQmlComponentIncubator(QQmlComponent *component, AppStartupMainWindowComponent *appExtra,
                             IncubationMode mode = AsynchronousIfNested);

protected:
    virtual void statusChanged(Status) override;
    virtual void setInitialState(QObject *) override;

private:
    AppStartupMainWindowComponent *mainComponent;
    QQmlComponent *compnent;
};

AppQmlComponentIncubator::AppQmlComponentIncubator(QQmlComponent *component, AppStartupMainWindowComponent *mainComponent, QQmlIncubator::IncubationMode mode)
    : QQmlIncubator(mode)
    , mainComponent(mainComponent)
    , compnent(component)
{

}

void AppQmlComponentIncubator::statusChanged(QQmlIncubator::Status status)
{
    if (status != QQmlIncubator::Ready) {
        if (status == QQmlIncubator::Error)
            qWarning() << "AppStartupInstance: " << this->errors();
        return;
    }

    QObject *obj = object();
    QQuickItem *item = qmlobject_cast<QQuickItem *>(obj);
    if (item) {
        item->setParentItem(mainComponent->appRootItem);
    }

    AppStartupInstanceAttached *attached = qobject_cast<AppStartupInstanceAttached*>(
        qmlAttachedPropertiesObject<AppStartupInstanceAttached>(mainComponent->appRootItem, false));

    if (attached) {
        QQmlContext *context = qmlContext(obj);
        attached->setSubObject(context->nameForObject(obj).toLocal8Bit(), obj);
    }

    mainComponent->_q_onComponentProgressChanged();
    mainComponent->destoryIncubator(this);
}

void AppQmlComponentIncubator::setInitialState(QObject *o)
{
    if (!mainComponent->appRootItem)
        return;

    if (o) {
        QQmlContext *context = mainComponent->itemContextMap.value(compnent);
        if (context)
            QQml_setParent_noEvent(context, o);
        QQml_setParent_noEvent(o, mainComponent->appRootItem);
        if (QQuickItem *item = qmlobject_cast<QQuickItem *>(o))
            item->setParentItem(mainComponent->appRootItem);
    }
}

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

void AppStartupComponent::transitionFinishedImpl(AppStartUpTransitionManager *transitionManager)
{
    transitionFinish();

    if (AppStartupComponent *linkTo = transitionLinkNext())
        linkTo->startTransition(TrasitionBeginMode::BeginCurrent);

    if (transitionManager)
        delete transitionManager;

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
        transitionFinishedImpl(nullptr);
        return false;
    }

    AppStartUpTransitionManager *transitionManager = new AppStartUpTransitionManager(nullptr);
    transitionManager->setFinishedCallback(std::bind(&AppStartupComponent::transitionFinishedImpl, this, transitionManager));
    transitionManager->transition({}, transition, transitionItem);
    return true;
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

void AppStartupPreloadComponent::doOverlayAutoExitChanged(AppStartupPreloadAttached *attached)
{
    if (attached->autoExitOverlay()) {
        if (autoExitConnection)
            QObject::disconnect(autoExitConnection);
    } else {
        autoExitConnection = QObject::connect(attached, &AppStartupPreloadAttached::overlayExitWhenChanged,
                            qq, std::bind(&AppStartupPreloadComponent::_q_onOverlayExitWhenChanged, this, std::placeholders::_1));
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

AppStartupComponent *AppStartupPreloadComponent::transitionLinkNext()
{
    return binder();
}

void AppStartupPreloadComponent::transitionFinish()
{
    clearOverlay();

    if (overlayExitTransition) {
        overlayExitTransition->deleteLater();
        overlayExitTransition = nullptr;
    }

    if (dd->defaultAppStartItem) {
        dd->defaultAppStartItem->setEnabled(true);
        dd->defaultAppStartItem->setVisible(true);
    }
}

void AppStartupPreloadComponent::load()
{
    const AppStartupComponentInformation &pluginInfo = this->information;
    QPluginLoader loader(pluginInfo.path());
    preloadInstance = qobject_cast<CCQuickStartupPreloadInterface *>(loader.instance());
    if (!preloadInstance) {
        //! @todo add error
        qFatal("load preload plugin failed!");
        return;
    }

    preloadInstance->aboutToPreload(dd->engine.get());
    QObject::connect(dd->engine.get(), &QQmlApplicationEngine::objectCreated,
                     qq, std::bind(&AppStartupPreloadComponent::_q_onPreloadCreated,
                    this, std::placeholders::_1, std::placeholders::_2));
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
                        qq, std::bind(&AppStartupPreloadComponent::_q_onWindowVisibleChanged,
                        this, std::placeholders::_1));
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

    QQmlComponent *loComponent = attached->loadingOverlay();
    if (!loComponent)
        return;

    QObject::connect(attached, &AppStartupPreloadAttached::autoExitOverlayChanged, qq,
                     std::bind(&AppStartupPreloadComponent::doOverlayAutoExitChanged, this, attached));
    doOverlayAutoExitChanged(attached);

    QQmlContext *context = creationContext(loComponent, dd->windowContentItem);
    loadingOverlay = qobject_cast<QQuickItem *>(loComponent->beginCreate(context));
    if (!loadingOverlay)
        return;

    loadingOverlay->setParentItem(dd->windowContentItem);
    loadingOverlay->setZ(100);

    overlayExitTransition = attached->overlayExited();
    if (overlayExitTransition)
        dd->engine->setObjectOwnership(overlayExitTransition, QQmlEngine::CppOwnership);

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

AppStartupComponent *AppStartupMainWindowComponent::transitionLinkPrev()
{
    return binder();
}

void AppStartupMainWindowComponent::transitionFinish()
{
    endOfTransition();

    if (rootDisplayTransition) {
        rootDisplayTransition->deleteLater();
        rootDisplayTransition = nullptr;
    }
}

void AppStartupMainWindowComponent::load()
{
    const AppStartupComponentInformation &pluginInfo = this->information;
    QPluginLoader loader(pluginInfo.path());
    mainInstance.reset(qobject_cast<CCQuickStartupMainWindowInterface *>(loader.instance()));
    if (!mainInstance) {
        //! @todo add error
        qWarning("load main window plugin failed!");
        return;
    }

    mainInstance->initialize(dd->engine.get());
    // Insert component into preload
    const QUrl &mainComponentPath = mainInstance->mainComponentPath();
    mainComponent = new QQmlComponent(dd->engine.get(), mainComponentPath, QQmlComponent::Asynchronous);
    if (mainComponent->isLoading()) {
        QObject::connect(mainComponent, &QQmlComponent::statusChanged,
                         qq, std::bind(&AppStartupMainWindowComponent::_q_onMainComponentStatusChanged,
                        this, std::placeholders::_1));
    } else {
        _q_onMainComponentStatusChanged(mainComponent->status());
    }
}

void AppStartupMainWindowComponent::finishedLoaded()
{
    mainInstance->finishedLoading(dd->engine.get());
    appRootItem->setLoaded(true);

    AppStartupInstanceAttached *itemAttached = qobject_cast<AppStartupInstanceAttached*>(qmlAttachedPropertiesObject<AppStartupInstanceAttached>(appRootItem, false));
    if (itemAttached)
        itemAttached->setLoaded(true);

    if (appRootItem == dd->defaultAppStartItem) {
        AppStartupPreloadAttached *preloadAttached = qobject_cast<AppStartupPreloadAttached*>(qmlAttachedPropertiesObject<AppStartupPreloadAttached>(dd->appWindow, false));
        if (preloadAttached)
            preloadAttached->setLoaded(true);

        Q_EMIT qq->loadFinished();

        if (!preloadAttached || preloadAttached->autoExitOverlay())
            startTransition();

        appRootItem->setFocus(true);
    }
}

void AppStartupMainWindowComponent::endOfTransition()
{
    if (appRootItem) {
        appRootItem->setEnabled(true);
        appRootItem->setFocus(true);
        appRootItem->setVisible(true);
    }
}

void AppStartupMainWindowComponent::destoryIncubator(QQmlIncubator *incubator)
{
    incubators.removeAll(incubator);
    childrenCount--;

    if (childrenCount == 0)
        finishedLoaded();

    incubator->clear();
    delete incubator;
}

void AppStartupMainWindowComponent::_q_onMainComponentStatusChanged(QQmlComponent::Status status)
{
    if (status != QQmlComponent::Ready) {
        if (status == QQmlComponent::Error)
            qWarning() << "AppStartupInstance: " << mainComponent->errors() << " " << mainComponent->errorString();
        return;
    }

    do {
        // ApplicationWindow.
        if (createObjects(APPLICATIONWINDOW_CONTENTDATA))
            break;

        // DialogWindow.
        if (createObjects(DIALOGWINDOW_CONTENTDATA))
            break;

        // Window.
        if (createObjects(WINDOW_CONTENTDATA))
            break;
    } while (false);

    mainComponent->deleteLater();
    mainComponent = nullptr;
}

void AppStartupMainWindowComponent::_q_onComponentProgressChanged()
{
    qreal progress = 0;
    auto components = appRootItem->findChildren<QQmlComponent *>();
    for (auto childCom : std::as_const(components)) {
        progress += childCom->progress();
    }

    appRootItem->setProgress(progress / components.count());
}

void AppStartupMainWindowComponent::itemGeometryChanged(QQuickItem *item, QQuickGeometryChange change, const QRectF &oldGeometry)
{
    updateRootItemSize(item);
    QQuickItemChangeListener::itemGeometryChanged(item, change, oldGeometry);
}

bool AppStartupMainWindowComponent::createObjects(const char *propertyName)
{
    Q_ASSERT(mainComponent);
    QQmlListReference pros(dd->appWindow, propertyName);
    if (!pros.isValid())
        return false;

    appRootItem = qobject_cast<QuickAppStartupItem *>(mainComponent->beginCreate(creationContext(mainComponent, dd->appWindow)));
    Q_ASSERT_X(appRootItem, "AppStartupInstance", "Must use the AppLoader item in main component.");
    appRootItem->setContainWindow(dd->appWindow);
    appRootItem->setEnabled(false);
    appRootItem->setVisible(false);
    mainComponent->completeCreate();

    if (dd->defaultComponentGroup.main() == this->information) {
        dd->defaultAppStartItem = appRootItem;

        AppStartupPreloadAttached *preloadAttached = qobject_cast<AppStartupPreloadAttached*>(qmlAttachedPropertiesObject<AppStartupPreloadAttached>(dd->appWindow, false));
        if (preloadAttached) {
            preloadAttached->setStartupItem(appRootItem);
        }
    }

    AppStartupInstanceAttached *itemAttached = qobject_cast<AppStartupInstanceAttached*>(qmlAttachedPropertiesObject<AppStartupInstanceAttached>(appRootItem, false));
    if (itemAttached)
        rootDisplayTransition = itemAttached->displayPopulate();

    if (rootDisplayTransition)
        dd->engine->setObjectOwnership(rootDisplayTransition, QQmlEngine::CppOwnership);

    pros.append(appRootItem);
    initRootItem(dd->windowContentItem);
    updateRootItemSize(dd->windowContentItem);
    createChildComponents();
    return true;
}

void AppStartupMainWindowComponent::createChildComponents()
{
    auto components = appRootItem->findChildren<QQmlComponent *>(QStringLiteral(""), Qt::FindDirectChildrenOnly);
    childrenCount = components.size();

    if (childrenCount == 0) {
        finishedLoaded();
        return;
    }

    for (auto childCom : std::as_const(components)) {
        QObject::connect(childCom, &QQmlComponent::progressChanged, qq,
                         std::bind(&AppStartupMainWindowComponent::_q_onComponentProgressChanged, this));
        auto asyn = appRootItem->asynchronous() ? AppQmlComponentIncubator::Asynchronous : AppQmlComponentIncubator::AsynchronousIfNested;
        AppQmlComponentIncubator *incubator = new AppQmlComponentIncubator(childCom, this, asyn);
        this->incubators.append(incubator);
        childCom->create(*incubator, creationContext(childCom, appRootItem));
    }
}

void AppStartupMainWindowComponent::updateRootItemSize(QQuickItem *item)
{
    if (!appRootItem)
        return;

    // The appRootItem always fills the window.
    QQuickItemPrivate *ip = QQuickItemPrivate::get(item);
    if (widthValid(ip)) {
        appRootItem->setWidth(item->width());
    } else {
        appRootItem->setWidth(dd->appWindow->width());
    }

    if (heightValid(ip)) {
        appRootItem->setHeight(item->height());
    } else {
        appRootItem->setWidth(dd->appWindow->height());
    }
}

AppStartupInstancePrivate::AppStartupInstancePrivate(AppStartupInstance *qq)
    : qq(qq)
    , engine (nullptr)
    , componentFactory(new AppStartupComponentFactory(this))
    , appWindow(nullptr)
{
    AppStartupInstance::self = qq;
}

void AppStartupInstancePrivate::scanPlugins()
{
    QList<AppStartupComponentInformation> plugins;

    // scan the dynamic plugins
    plugins += scanDynamicPlugins();
    // scan the static plugins
    plugins += scanStaticPlugins();

    std::sort(plugins.begin(), plugins.end(),
              [](const AppStartupComponentInformation &before, const AppStartupComponentInformation &after) {
        return before.startComponent() < after.startComponent()
               || before.descriptor() < after.descriptor()
               || before.version() < after.version();
    });

    detachAvailablePluginsChange(plugins);
}

QList<AppStartupComponentInformation> AppStartupInstancePrivate::scanDynamicPlugins()
{
    QList<AppStartupComponentInformation> plugins;

    for (auto iter = pluginPaths.rbegin(); iter != pluginPaths.rend(); ++iter) {
        const QDir dir(*iter);
        if (!dir.exists())
            continue;

        for (const auto &entry : dir.entryInfoList(QDir::Files | QDir::NoSymLinks)) {
            const auto path = entry.absoluteFilePath();
            if (!QLibrary::isLibrary(path))
                continue;

            QPluginLoader loader(path);
            const QJsonObject &metaData = loader.metaData();

            AppStartupComponentInformation info;
            if (!resolveMetaInfoFromObject(metaData, &info))
                continue;

            info.setPath(path);
            qInfo("Find the available plugin %s:%s from the plugin path: %s", qPrintable(info.descriptor()),
                  qPrintable(info.version()), qPrintable(path));

            plugins << info;
        }
    }

    return plugins;
}

QList<AppStartupComponentInformation> AppStartupInstancePrivate::scanStaticPlugins()
{
    QList<AppStartupComponentInformation> plugins;

    auto static_plugins = QPluginLoader::staticPlugins();
    for (auto splg : std::as_const(static_plugins)) {

        const auto &metaData = splg.metaData();
        AppStartupComponentInformation info;
        if (!resolveMetaInfoFromObject(metaData, &info))
            continue;

        qInfo("Find the available plugin %s:%s from the static plugins", qPrintable(info.descriptor()),
              qPrintable(info.version()));

        plugins << info;
    }

    return plugins;
}

void AppStartupInstancePrivate::detachAvailablePluginsChange(const QList<AppStartupComponentInformation> &plugins)
{
    QList<AppStartupComponentGroup> groupList;
    bool availableChanged = false;

    int left = 0;
    int rightLimit = -1;
    while (left < plugins.size() - 1) {
        if ((left > 0 && (plugins[left].startComponent() != plugins[left - 1].startComponent())))
            break;

        if (plugins[left].startComponent() == plugins[left + 1].startComponent()
            && plugins[left].descriptor() == plugins[left + 1].descriptor()) {
            left++;
            continue;
        }

        int right = plugins.size() - 1;
        while ((rightLimit >= 0 ? (right < rightLimit)
                                : (plugins[right].startComponent() != plugins[left].startComponent()))
               && plugins[right].descriptor() != plugins[left].descriptor()) {
            right--;
        }

        const QString &desciptor = plugins[right].descriptor();
        if (desciptor == plugins[left].descriptor()) {
            AppStartupComponentGroup group({plugins[left], plugins[right]});
            if (!availableChanged && !availablePlugins.contains(group)) {
                availableChanged = true;
            }

            groupList.append(group);
            rightLimit = right;
        }

        left++;
    }

    if (availableChanged || groupList.size() != availablePlugins.size()) {
        availablePlugins = groupList;
        Q_EMIT qq->availablePluginsChanged();
    }
}

void AppStartupInstancePrivate::reloadPlugins()
{
    if (reloadPluginsList.isEmpty())
        return;

    unloadPlugins();
    findDefaultComponentGroup();
    loadPreloadPlugins();
}

void AppStartupInstancePrivate::unloadPlugins()
{
    if (this->componentPluginHash.isEmpty())
        return;

    if (appWindow) {
        appWindow->close();
        appWindow->destroy();
        appWindow->deleteLater();
        appWindow = nullptr;
        windowContentItem.clear();
    }

    qmlClearTypeRegistrations();
    defaultAppStartItem.clear();
    defaultComponentGroup = {};

    if (engine) {
        engine->clearSingletons();
        engine->clearComponentCache();
    }

    // unload the exist plugins
    auto it = componentPluginHash.begin();
    while (it != componentPluginHash.end()) {
        QString pluginPath = it.key().path();
        it = componentPluginHash.erase(it);

        if (!pluginPath.isEmpty()) {
            QPluginLoader loader(pluginPath);
            if (loader.isLoaded()) {
                qInfo() << "Unload the plugin: " << it.key().appId() << ", from path: "
                        << pluginPath;
                loader.unload();
            }
        }
    }
}

void AppStartupInstancePrivate::findDefaultComponentGroup()
{
    for (const auto &group : reloadPluginsList) {
        if (!group.isValid() || !group.preload().isDefault() || !group.main().isDefault())
            continue;

        this->defaultComponentGroup = group;
        return;
    }
}

void AppStartupInstancePrivate::loadMainWindowPlugins()
{
    while (!reloadPluginsList.isEmpty()) {
        const auto &group = reloadPluginsList.takeFirst();
        AppStartupComponent *component = componentFactory->create(group.main());
        if (!component)
            continue;

        auto preloadComponent = this->componentPluginHash.value(group.preload());
        if (preloadComponent) {
            component->setBinder(preloadComponent.get());
            preloadComponent->setBinder(component);
        }

        this->componentPluginHash.insert(group.main(), QSharedPointer<AppStartupComponent>(component));
        component->load();
    }
}

void AppStartupInstancePrivate::loadPreloadPlugins()
{
    if (!defaultComponentGroup.isValid()) {
        //! @todo add error
        qFatal("No preload plugin found when load the exist plugins!");
        return;
    }

    AppStartupComponent *component = componentFactory->create(defaultComponentGroup.preload());
    if (!component) {
        //! @todo add error
        qFatal("Create preload component failed!");
        return;
    }

    this->componentPluginHash.insert(defaultComponentGroup.preload(), QSharedPointer<AppStartupComponent>(component));
    component->load();
}

bool AppStartupInstancePrivate::resolveMetaInfoFromObject(const QJsonObject &obj, AppStartupComponentInformation *info)
{
    if (!info)
        return false;

    auto metaDataValue = obj.value("MetaData");
    if (!metaDataValue.isObject())
        return false;

    const QString iid = obj["IID"].toString();
    if (iid != qobject_interface_iid<CCQuickStartupPreloadInterface *>()
        && iid != qobject_interface_iid<CCQuickStartupMainWindowInterface *>())
        return false;

    auto metaDataObject = metaDataValue.toObject();
    auto pluginAppId = metaDataObject.value("appid").toString();
    if (pluginAppId != this->appId)
        return false;

    info->setStartComponent(iid == qobject_interface_iid<CCQuickStartupPreloadInterface *>()
                                ? AppStartupComponentInformation::Preload
                                : AppStartupComponentInformation::MainWindow);
    info->setAppId(pluginAppId);
    info->setDescriptor(metaDataObject.value("descriptor").toString());
    info->setVersion(metaDataObject.value("version").toString());
    info->setDescription(metaDataObject.value("description").toString());
    QStringList featuresList;
    const auto &featuresArray = metaDataObject.value("features").toArray();
    for (auto featureValue : featuresArray) {
        featuresList << featureValue.toString();
    }
    info->setFeatures(featuresList);
    info->setChangelog(metaDataObject.value("changelog").toString());
    info->setDefault(metaDataObject.value("default").toBool());

    return true;
}

QStringList AppStartupInstancePrivate::buildinPluginPaths()
{
    QStringList result;
    const auto orangePluginPath = qgetenv("ORANGE_QML_PLUGIN_PATH");
    if (!orangePluginPath.isEmpty())
        result.append(orangePluginPath);

#ifdef CC_QML_APP_PLUGIN_PATH
    result.append(ORANGE_QML_APP_PLUGIN_PATH);
#endif

    return result;
}

AppStartupInstance::AppStartupInstance(const QString &appId, const QString &appPath, QObject *parent)
    : QObject (parent)
    , dd(new AppStartupInstancePrivate(this))
{
    dd->appId = appId;

    const auto &paths = dd->buildinPluginPaths();
    for (auto iter = paths.rbegin(); iter != paths.rend(); iter++)
        addPluginPath(*iter);
    addPluginPath(appPath);
}

AppStartupInstance::~AppStartupInstance()
{
    dd->componentPluginHash.clear();
    self = nullptr;
}

void AppStartupInstance::addPluginPath(const QString &dir)
{
    if (dir.isEmpty())
        return;
    dd->pluginPaths.append(dir);
}

QStringList AppStartupInstance::pluginPaths() const
{
    QStringList paths;
    std::reverse_copy(dd->pluginPaths.begin(), dd->pluginPaths.end(), std::back_inserter(paths));
    return paths;
}

void AppStartupInstance::scanPlugins()
{
    dd->scanPlugins();
}

QList<AppStartupComponentGroup> AppStartupInstance::availablePlugins() const
{
    return dd->availablePlugins;
}

QList<AppStartupComponentGroup> AppStartupInstance::loadedPlugins() const
{
    return dd->loadedPluginsList;
}

void AppStartupInstance::addReloadPlugin(const AppStartupComponentGroup &plugin)
{
    if (dd->reloadPluginsList.contains(plugin))
        return;

    dd->reloadPluginsList.append(plugin);
}

void AppStartupInstance::removeReloadPlugin(const AppStartupComponentGroup &plugin)
{
    if (!dd->reloadPluginsList.contains(plugin))
        return;

    dd->reloadPluginsList.removeOne(plugin);
}

void AppStartupInstance::reload()
{
    dd->reloadPlugins();
}

int AppStartupInstance::exec(int &argc, char **argv)
{
    dd->scanPlugins();
    dd->reloadPluginsList = dd->availablePlugins;
    if (dd->reloadPluginsList.empty()) {
        qFatal("No available plugins found!");
        return -1;
    }

    if (!dd->app)
        dd->app.reset(new QGuiApplication(argc, argv));

    if (!dd->engine)
        dd->engine.reset(new QQmlApplicationEngine(this));

    dd->reloadPlugins();
    return dd->app->exec();
}

#include "moc_appstartupinstance.cpp"
