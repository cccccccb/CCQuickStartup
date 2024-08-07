#include "framelessattached.h"
#include "frameless.h"

#include <QQuickItem>

FramelessAttached::FramelessAttached(QQuickWindow *parent)
    : QObject{parent}
    , m_frameless(new Frameless(parent, this))
{
}

QQmlListProperty<QQuickItem> FramelessAttached::moveUnder()
{
    return {this, &m_moveUnderContainer};
}

QQmlListProperty<QQuickItem> FramelessAttached::moveExclude()
{
    return {this, &m_moveExcludeContainer};
}

int FramelessAttached::frameBorder() const
{
    return m_frameless->framelessBorder();
}

void FramelessAttached::setFrameBorder(int newFrameBorder)
{
    if (m_frameless->framelessBorder() == newFrameBorder)
        return;
    m_frameless->setFramelessBorder(newFrameBorder);
    emit frameBorderChanged();
}

bool FramelessAttached::canWindowResize() const
{
    return m_frameless->canWindowResize();
}

void FramelessAttached::setCanWindowResize(bool newCanWindowResize)
{
    if (m_frameless->canWindowResize() == newCanWindowResize)
        return;
    m_frameless->setCanWindowResize(newCanWindowResize);
    emit canWindowResizeChanged();
}


bool FramelessAttached::enabled() const
{
    return m_frameless->enabled();
}

void FramelessAttached::setEnabled(bool newEnable)
{
    if (m_frameless->enabled() == newEnable)
        return;

    if (newEnable) {
        m_frameless->window()->installEventFilter(this);
    } else {
        m_frameless->window()->removeEventFilter(this);
    }

    m_frameless->setEnabled(newEnable);
    emit enabledChanged();
}

qreal FramelessAttached::contentMargins() const
{
    return m_frameless->contentMargins();
}

void FramelessAttached::setContentMargins(qreal newContentMargins)
{
    if (qFuzzyCompare(m_frameless->contentMargins(), newContentMargins))
        return;
    m_frameless->setContentMargins(newContentMargins);
    emit contentMarginsChanged();
}

void FramelessAttached::activateWindow()
{
    m_frameless->window()->setWindowStates(m_frameless->window()->windowStates() & ~Qt::WindowMinimized);
    m_frameless->window()->raise();
    m_frameless->window()->requestActivate();
}

FramelessAttached *FramelessAttached::qmlAttachedProperties(QObject *object)
{
    if (QQuickWindow *window = qobject_cast<QQuickWindow *>(object)) {
        return new FramelessAttached(window);
    }

    return nullptr;
}

bool FramelessAttached::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == m_frameless->window()) {
        if (event->type() == QEvent::MouseButtonPress) {
            auto underIncludeContainer = std::find_if(m_moveUnderContainer.begin(), m_moveUnderContainer.end(),
                    [](QQuickItem *item) {
                return item->isUnderMouse() && item->isEnabled();
            });

            auto underExcludeContainer = std::find_if(m_moveExcludeContainer.begin(), m_moveExcludeContainer.end(),
                                          [](QQuickItem *item) {
                return item->isUnderMouse() && item->isEnabled();
            });

            if (underIncludeContainer != m_moveUnderContainer.end()) {
                m_frameless->setCanWindowMove((underExcludeContainer == m_moveExcludeContainer.end())
                                              || ((*underIncludeContainer)->isAncestorOf((*underExcludeContainer))
                                                      ? false
                                                      : ((*underIncludeContainer)->z() > (*underExcludeContainer)->z())));
            } else {
                m_frameless->setCanWindowMove(false);
            }
        }

        m_frameless->targetEvent(event);
    }

    return QObject::eventFilter(watched, event);
}
