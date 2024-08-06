#include "appstartupitemattached.h"

class AppStartupItemAttachedPrivate {
public:
    AppStartupItemAttachedPrivate(AppStartupItemAttached *qq)
        : _qq(qq)
    {}

    AppStartupItemAttached *_qq;
    bool _loaded = false;
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

#include "moc_appstartupitemattached.cpp"

