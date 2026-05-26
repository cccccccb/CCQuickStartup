#include "appstartupinterface.h"
#include "appstartupitem.h"
#include "appstartupitemattached.h"

static AppStartupItem *findStartupRootItem(QObject *obj)
{
    do {
        AppStartupItem *item = qobject_cast<AppStartupItem *>(obj);
        if (item)
            return item;

        obj = obj->parent();
    } while (obj);

    return nullptr;
}

static AppStartupItem *findStartupRootItem(QQmlContext *context)
{
    do {
        AppStartupItem *item = qobject_cast<AppStartupItem *>(context->contextObject());
        if (item)
            return item;

        context = context->parentContext();
    } while (context);

    return nullptr;
}


class AppStartupItemPrivate {
public:
    AppStartupItemPrivate(AppStartupItem *qq)
        : _qq(qq)
    {}

    const QString &propertyName(int index) const;

    AppStartupItem *_qq;
    QQuickItem *_container = nullptr;
    bool _loaded = false;
    bool _asynchronous = false;
    qreal _progress = 0.0;
    bool _populate = false;
    QVector<QObject *> _objects;
    QVector<AppStartupInterface *> _interfaces;

    QPointer<AppStartupItemAttached> _attached;

    QStringList keys;
};

const QString &AppStartupItemPrivate::propertyName(int index) const
{
    if (index < 0 || index >= keys.size()) {
        static const QString empty;
        return empty;
    }
    return keys[index];
}

AppStartupItem::AppStartupItem(QQuickItem *parentItem)
    : QQuickItem(parentItem)
    , dd(new AppStartupItemPrivate(this))
{
}

AppStartupItem::~AppStartupItem()
{

}

bool AppStartupItem::loaded() const
{
    return dd->_loaded;
}

void AppStartupItem::setLoaded(bool loaded)
{
    if (loaded == dd->_loaded)
        return;
    dd->_loaded = loaded;
    Q_EMIT loadedChanged();
}

qreal AppStartupItem::progress() const
{
    return dd->_progress;
}

void AppStartupItem::setProgress(qreal progress)
{
    if (progress < 0.0)
        progress = 0.0;
    else if (progress > 1.0)
        progress = 1.0;
    if (qFuzzyCompare(progress, dd->_progress))
        return;
    dd->_progress = progress;
    Q_EMIT progressChanged();
}

QQuickItem *AppStartupItem::container() const
{
    return dd->_container;
}

void AppStartupItem::setContainer(QQuickItem *c)
{
    if (c == dd->_container)
        return;
    dd->_container = c;
    Q_EMIT containerChanged();
}

bool AppStartupItem::asynchronous() const
{
    return dd->_asynchronous;
}

void AppStartupItem::setAsynchronous(bool a)
{
    if (a == dd->_asynchronous)
        return;
    dd->_asynchronous = a;
    Q_EMIT asynchronousChanged();
}

bool AppStartupItem::populate() const
{
    return dd->_populate;
}

void AppStartupItem::setPopulate(bool populate)
{
    if (populate == dd->_populate)
        return;
    dd->_populate = populate;
    Q_EMIT populateChanged();
}

QQmlListProperty<QObject> AppStartupItem::objects()
{
    return {this, &dd->_objects};
}

QQmlListProperty<AppStartupInterface> AppStartupItem::interfaces()
{
    return {this, &dd->_interfaces};
}

AppStartupItemAttached *AppStartupItem::qmlAttachedProperties(QObject *object)
{
    AppStartupItemAttached *attached = nullptr;

    AppStartupItem *appRootItem = findStartupRootItem(object);
    if (!appRootItem)
        appRootItem = findStartupRootItem(qmlContext(object));

    if (appRootItem) {
        attached = appRootItem->dd->_attached;

        if (!attached) {
            attached = new AppStartupItemAttached(appRootItem);
            attached->setStartupItem(appRootItem);
            appRootItem->dd->_attached = attached;
        }
    }
    return attached;
}

#include "moc_appstartupitem.cpp"
