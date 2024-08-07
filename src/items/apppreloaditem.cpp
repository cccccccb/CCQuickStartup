#include "apppreloaditem.h"
#include "appstartupitem.h"
#include "appstartupinitialproperties.h"

#include <QQmlComponent>
#include <QQuickWindow>

class AppPreloadItemPrivate {
public:
    AppPreloadItemPrivate(AppPreloadItem *qq)
        : _qq(qq)
        , _startupItem(new AppStartupItem(nullptr))
    {}

    AppPreloadItem *_qq;
    AppPreloadItem::PreloadView _preloadView;
    QQmlComponent *_customPreloadView = nullptr;
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

AppPreloadItem::PreloadView AppPreloadItem::preloadView() const
{
    return dd->_preloadView;
}

void AppPreloadItem::setPreloadView(PreloadView preview)
{
    if (dd->_preloadView == preview)
        return;

    dd->_preloadView = preview;
    Q_EMIT preloadViewChanged();
}

QQmlComponent *AppPreloadItem::customPreloadView() const
{
    return dd->_customPreloadView;
}

void AppPreloadItem::setCustomPreloadView(QQmlComponent *custom)
{
    if (dd->_customPreloadView == custom)
        return;

    dd->_customPreloadView = custom;
    Q_EMIT customPreloadViewChanged();
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

#include "moc_apppreloaditem.cpp"
