#include "quickappstartupitem.h"

QuickAppStartupItem::QuickAppStartupItem(QQuickItem *parentItem)
    : QQuickItem(parentItem)
{

}

bool QuickAppStartupItem::loaded() const
{
    return _loaded;
}

void QuickAppStartupItem::setLoaded(bool loaded)
{
    if (loaded == this->_loaded)
        return;
    this->_loaded = loaded;
    Q_EMIT loadedChanged();
}

qreal QuickAppStartupItem::progress() const
{
    return this->_progress;
}

void QuickAppStartupItem::setProgress(qreal progress)
{
    if (qFuzzyCompare(progress, this->_progress))
        return;
    this->_progress = progress;
    Q_EMIT progressChanged();
}

QQuickWindow *QuickAppStartupItem::containWindow() const
{
    return this->_window;
}

void QuickAppStartupItem::setContainWindow(QQuickWindow *w)
{
    if (w == this->_window)
        return;
    this->_window = w;
    Q_EMIT containWindowChanged();
}

bool QuickAppStartupItem::asynchronous() const
{
    return this->_asynchronous;
}

void QuickAppStartupItem::setAsynchronous(bool a)
{
    if (a == this->_asynchronous)
        return;
    this->_asynchronous = a;
    Q_EMIT asynchronousChanged();
}
