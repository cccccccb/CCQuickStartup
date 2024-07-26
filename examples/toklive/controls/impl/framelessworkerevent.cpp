#include "FramelessWorkerEvent.h"

FramelessEvent::FramelessEvent(EventType type)
    : mEventType(type)
{

}

FramelessEvent::~FramelessEvent()
{

}

FramelessEvent::EventType FramelessEvent::type()
{
    return mEventType;
}

FramelessFocusInEvent::FramelessFocusInEvent()
    : FramelessEvent(EventType::FocusIn)
{

}

FramelessMousePressEvent::FramelessMousePressEvent()
    : FramelessEvent(EventType::MousePress)
{

}

FramelessMouseMoveEvent::FramelessMouseMoveEvent()
    : FramelessEvent(EventType::MouseMove)
{

}

FramelessMouseReleaseEvent::FramelessMouseReleaseEvent()
    : FramelessEvent(EventType::MouseRelease)
{

}

FramelessLeaveEvent::FramelessLeaveEvent()
    : FramelessEvent(EventType::Leave)
{

}
