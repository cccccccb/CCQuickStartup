#ifndef FRAMELESSWORKEREVENT_H
#define FRAMELESSWORKEREVENT_H

#include <QPoint>
#include <QPointer>

class Frameless;
class QWindow;
struct FramelessEvent
{
    enum EventType {
        UnkonwEvent = -1,
        FocusIn,
        MouseHover,
        MousePress,
        MouseMove,
        MouseRelease,
        Leave,
    };

    FramelessEvent(EventType type);
    virtual ~FramelessEvent();

    EventType type();
    QWindow *target = nullptr;
    QPointer<Frameless> frameless;

private:
    EventType mEventType = EventType::UnkonwEvent;
};

struct FramelessFocusInEvent : public FramelessEvent
{
    FramelessFocusInEvent();

    bool canWindowResize = true;
};

struct FramelessMousePressEvent : public FramelessEvent
{
    FramelessMousePressEvent();

    QPoint globalCursorPositon;
    bool canWindowMove = false;
};

struct FramelessMouseMoveEvent : public FramelessEvent
{
    FramelessMouseMoveEvent();

    QPoint globalCursorPositon;
    bool canWindowResize = true;
};

struct FramelessMouseReleaseEvent : public FramelessEvent
{
    FramelessMouseReleaseEvent();
};

struct FramelessLeaveEvent : public FramelessEvent
{
    FramelessLeaveEvent();
};

struct FramelessWindowDeactivateEvent : FramelessEvent
{
    FramelessWindowDeactivateEvent();
};
#endif // FRAMELESSWORKEREVENT_H
