#include "appstartuppreloaditem.h"
#include "appstartupinitialproperties.h"
#include "appstartupitem.h"

#include <QQmlComponent>
#include <QQuickWindow>

class AppPreloadItemPrivate {
public:
    AppPreloadItemPrivate(AppPreloadItem *qq)
        : _qq(qq)
        , _startupItem(new AppStartupItem(nullptr))
    {}

    AppPreloadItem *_qq;
    AppPreloadItem::PreloadSurface _preloadSurface = AppPreloadItem::WindowSurface;
    QQmlComponent *_customPreloadSurface = nullptr;
    QQmlComponent *_transitionGroup = nullptr;
    QQmlComponent *_loadingOverlay = nullptr;
    AppStartupItem *_startupItem = nullptr;
    bool _loaded = false;
    bool _autoExitOverlay = true;
    bool _overlayExitWhen = false;
    AppStartupInitialProperties *_initialProperties = nullptr;
    QQuickWindow *_window = nullptr;
};

AppPreloadItem::AppPreloadItem(QQuickItem *parentItem)
    : QQuickItem(parentItem)
    , dd(new AppPreloadItemPrivate(this))
{

}

AppPreloadItem::~AppPreloadItem()
{

}

AppPreloadItem::PreloadSurface AppPreloadItem::preloadSurface() const
{
    return dd->_preloadSurface;
}

void AppPreloadItem::setPreloadSurface(PreloadSurface surface)
{
    if (dd->_preloadSurface == surface)
        return;

    dd->_preloadSurface = surface;
    Q_EMIT preloadSurfaceChanged();
}

QQmlComponent *AppPreloadItem::customPreloadSurface() const
{
    return dd->_customPreloadSurface;
}

void AppPreloadItem::setCustomPreloadSurface(QQmlComponent *custom)
{
    if (dd->_customPreloadSurface == custom)
        return;

    dd->_customPreloadSurface = custom;
    Q_EMIT customPreloadSurfaceChanged();
}

QQmlComponent *AppPreloadItem::transitionGroup() const
{
    return dd->_transitionGroup;
}

void AppPreloadItem::setTransitionGroup(QQmlComponent *transitionGroup)
{
    if (dd->_transitionGroup == transitionGroup)
        return;
    dd->_transitionGroup = transitionGroup;
    Q_EMIT transitionGroupChanged();
}

QQmlComponent *AppPreloadItem::loadingOverlay() const
{
    return dd->_loadingOverlay;
}

void AppPreloadItem::setLoadingOverlay(QQmlComponent *component)
{
    if (component == dd->_loadingOverlay)
        return;

    dd->_loadingOverlay = component;
    Q_EMIT loadingOverlayChanged();
}

bool AppPreloadItem::autoExitOverlay() const
{
    return dd->_autoExitOverlay;
}

void AppPreloadItem::setAutoExitOverlay(bool autoExitOverlay)
{
    if (autoExitOverlay == dd->_autoExitOverlay)
        return;

    dd->_autoExitOverlay = autoExitOverlay;
    Q_EMIT autoExitOverlayChanged(autoExitOverlay);
}

bool AppPreloadItem::overlayExitWhen() const
{
    return dd->_overlayExitWhen;
}

void AppPreloadItem::setOverlayExitWhen(bool overlayExitWhen)
{
    if (overlayExitWhen == dd->_overlayExitWhen)
        return;

    dd->_overlayExitWhen = overlayExitWhen;
    Q_EMIT overlayExitWhenChanged(overlayExitWhen);
}

AppStartupItem *AppPreloadItem::startupItem() const
{
    return dd->_startupItem;
}

void AppPreloadItem::setStartupItem(AppStartupItem *item)
{
    if (dd->_startupItem == item)
        return;

    if (dd->_startupItem)
        dd->_startupItem->deleteLater();

    dd->_startupItem = item;
    Q_EMIT startupItemChanged();
}


bool AppPreloadItem::loaded() const
{
    return dd->_loaded;
}

void AppPreloadItem::setLoaded(bool loaded)
{
    if (dd->_loaded == loaded)
        return;

    dd->_loaded = loaded;
    Q_EMIT loadedChanged();
}

AppStartupInitialProperties *AppPreloadItem::initialProperties() const
{
    return dd->_initialProperties;
}

void AppPreloadItem::setInitialProperties(AppStartupInitialProperties *initialProperties)
{
    if (dd->_initialProperties == initialProperties)
        return;

    dd->_initialProperties = initialProperties;
    Q_EMIT initialPropertiesChanged();
}

void AppPreloadItem::setWindow(QQuickWindow *window)
{
    if (dd->_window == window)
        return;

    dd->_window = window;
    Q_EMIT windowChanged();
}

QQuickWindow *AppPreloadItem::window() const
{
    return dd->_window;
}

#include "moc_appstartuppreloaditem.cpp"
