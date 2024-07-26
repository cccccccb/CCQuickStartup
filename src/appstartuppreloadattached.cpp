#include "appstartuppreloadattached.h"
#include "quickappstartupitem.h"

#include <QQuickWindow>
#include <private/qquicktransition_p.h>

class AppStartupPreloadAttachedPrivate {
public:
    AppStartupPreloadAttachedPrivate(AppStartupPreloadAttached *qq)
        : _qq(qq)
        , _startupItem(new QuickAppStartupItem(nullptr))
    {}

    AppStartupPreloadAttached *_qq;
    QQuickTransition *_overlayExitedTransition = nullptr;
    QQmlComponent *_loadingOverlay = nullptr;
    QuickAppStartupItem *_startupItem = nullptr;
    bool _loaded = false;
    bool _autoExitOverlay = true;
    bool _overlayExitWhen = false;
};

AppStartupPreloadAttached::AppStartupPreloadAttached(QObject *parent)
    : QObject(parent)
    , dd(new AppStartupPreloadAttachedPrivate(this))
{}

AppStartupPreloadAttached::~AppStartupPreloadAttached()
{

}

QQuickTransition *AppStartupPreloadAttached::overlayExited() const
{
    return dd->_overlayExitedTransition;
}

void AppStartupPreloadAttached::setOverlayExited(QQuickTransition *exit)
{
    if (dd->_overlayExitedTransition == exit)
        return;
    dd->_overlayExitedTransition = exit;
    Q_EMIT overlayExitedChanged();
}

QQmlComponent *AppStartupPreloadAttached::loadingOverlay() const
{
    return dd->_loadingOverlay;
}

void AppStartupPreloadAttached::setLoadingOverlay(QQmlComponent *component)
{
    if (component == dd->_loadingOverlay)
        return;

    dd->_loadingOverlay = component;
    Q_EMIT loadingOverlayChanged();
}

bool AppStartupPreloadAttached::autoExitOverlay() const
{
    return dd->_autoExitOverlay;
}

void AppStartupPreloadAttached::setAutoExitOverlay(bool autoExitOverlay)
{
    if (autoExitOverlay == dd->_autoExitOverlay)
        return;

    dd->_autoExitOverlay = autoExitOverlay;
    Q_EMIT autoExitOverlayChanged(autoExitOverlay);
}

bool AppStartupPreloadAttached::overlayExitWhen() const
{
    return dd->_overlayExitWhen;
}

void AppStartupPreloadAttached::setOverlayExitWhen(bool overlayExitWhen)
{
    if (overlayExitWhen == dd->_overlayExitWhen)
        return;

    dd->_overlayExitWhen = overlayExitWhen;
    Q_EMIT overlayExitWhenChanged(overlayExitWhen);
}

QuickAppStartupItem *AppStartupPreloadAttached::startupItem() const
{
    return dd->_startupItem;
}

void AppStartupPreloadAttached::setStartupItem(QuickAppStartupItem *item)
{
    if (dd->_startupItem == item)
        return;

    if (dd->_startupItem)
        dd->_startupItem->deleteLater();

    dd->_startupItem = item;
    Q_EMIT startupItemChanged();
}


bool AppStartupPreloadAttached::loaded() const
{
    return dd->_loaded;
}

void AppStartupPreloadAttached::setLoaded(bool loaded)
{
    if (dd->_loaded == loaded)
        return;

    dd->_loaded = loaded;
    Q_EMIT loadedChanged();
}

AppStartupPreloadAttached *AppStartupPreloadAttached::qmlAttachedProperties(QObject *object)
{
    AppStartupPreloadAttached *attached = nullptr;

    if (QQuickWindow *window = qobject_cast<QQuickWindow *>(object)) {
        attached = new AppStartupPreloadAttached(window);
    }

    return attached;
}

#include "moc_appstartuppreloadattached.cpp"
