#include "appstartupitem.h"

AppStartupItem::AppStartupItem(QQuickItem *parentItem)
    : QQuickItem(parentItem)
{

}

bool AppStartupItem::loaded() const
{
    return _loaded;
}

void AppStartupItem::setLoaded(bool loaded)
{
    if (loaded == this->_loaded)
        return;
    this->_loaded = loaded;
    Q_EMIT loadedChanged();
}

qreal AppStartupItem::progress() const
{
    return this->_progress;
}

void AppStartupItem::setProgress(qreal progress)
{
    if (qFuzzyCompare(progress, this->_progress))
        return;
    this->_progress = progress;
    Q_EMIT progressChanged();
}

QQuickWindow *AppStartupItem::containWindow() const
{
    return this->_window;
}

void AppStartupItem::setContainWindow(QQuickWindow *w)
{
    if (w == this->_window)
        return;
    this->_window = w;
    Q_EMIT containWindowChanged();
}

bool AppStartupItem::asynchronous() const
{
    return this->_asynchronous;
}

void AppStartupItem::setAsynchronous(bool a)
{
    if (a == this->_asynchronous)
        return;
    this->_asynchronous = a;
    Q_EMIT asynchronousChanged();
}
