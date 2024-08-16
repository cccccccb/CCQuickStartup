#include "appstartupitemattached.h"
#include "appstartupitem.h"
#include "appstartupcomponentinformation.h"

class AppStartupItemAttachedPrivate {
public:
    AppStartupItemAttachedPrivate(AppStartupItemAttached *qq)
        : _qq(qq)
    {}

    AppStartupItemAttached *_qq;
    bool _loaded = false;
    AppStartupItem *_startupItem = nullptr;
    QMap<QByteArray, QObject *> _objects;
};

AppStartupItemAttached::AppStartupItemAttached(QObject *parent)
    : QQmlPropertyMap(this, parent)
    , dd(new AppStartupItemAttachedPrivate(this))
{
}

AppStartupItemAttached::~AppStartupItemAttached()
{

}

bool AppStartupItemAttached::loaded() const
{
    return dd->_loaded;
}

void AppStartupItemAttached::setLoaded(bool loaded)
{
    if (dd->_loaded == loaded)
        return;

    dd->_loaded = loaded;
    Q_EMIT loadedChanged();
}

AppStartupItem *AppStartupItemAttached::startupItem() const
{
    return dd->_startupItem;
}

void AppStartupItemAttached::setStartupItem(AppStartupItem *item)
{
    if (dd->_startupItem == item)
        return;

    dd->_startupItem = item;
    Q_EMIT startupItemChanged();
}

void AppStartupItemAttached::loadComponent(const AppStartupComponentGroup &information, QQuickItem *container)
{

}

void AppStartupItemAttached::unloadComponent(const AppStartupComponentGroup &information)
{

}

QQuickItem *AppStartupItemAttached::componentContainer(const AppStartupComponentGroup &information) const
{
    return nullptr;
}

#include "moc_appstartupitemattached.cpp"
