#include "appstartupitem.h"

class AppStartupItemPrivate {
public:
    AppStartupItemPrivate(AppStartupItem *qq)
        : _qq(qq)
    {}

    QVariant updateValue(const QString &key, const QVariant &input);
    bool validKeyName(const QString& name);

    const QString &propertyName(int index) const;

    AppStartupItem *_qq;
    QList<QQmlComponent *> _childrenComponents;
    QQuickWindow *_window = nullptr;
    bool _loaded = false;
    bool _asynchronous = false;
    qreal _progress = 0.0;

    QStringList keys;
};

bool AppStartupItemPrivate::validKeyName(const QString& name)
{
    //The following strings shouldn't be used as property names
    return  name != QLatin1String("loaded")
           && name != QLatin1String("progress")
           && name != QLatin1String("containWindow")
           && name != QLatin1String("asynchronous")
           && name != QLatin1String("QObject")
           && name != QLatin1String("destroyed")
           && name != QLatin1String("deleteLater");
}

QVariant AppStartupItemPrivate::updateValue(const QString &, const QVariant &input)
{
    return input;
}

const QString &AppStartupItemPrivate::propertyName(int index) const
{
    Q_ASSERT(index < keys.size());
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
    if (qFuzzyCompare(progress, dd->_progress))
        return;
    dd->_progress = progress;
    Q_EMIT progressChanged();
}

QQuickWindow *AppStartupItem::containWindow() const
{
    return dd->_window;
}

void AppStartupItem::setContainWindow(QQuickWindow *w)
{
    if (w == dd->_window)
        return;
    dd->_window = w;
    Q_EMIT containWindowChanged();
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

#include "moc_appstartupitem.cpp"
