#include "FramelessWorker.h"
#include "Frameless.h"
#include "FramelessWorkerEvent.h"

#include <QGuiApplication>
#include <QDebug>
#include <QWindow>
#include <QTimer>

FramelessWorker *FramelessWorker::mInstance = nullptr;

FramelessWorker::FramelessWorker(QObject *parent)
    : QThread{parent}
    , mExit(false)
{
    connect(qApp, &QGuiApplication::aboutToQuit, this, &FramelessWorker::exit);
}

FramelessWorker *FramelessWorker::instance()
{
    if (mInstance)
        return mInstance;

    mInstance = new FramelessWorker;
    return mInstance;
}

void FramelessWorker::exit()
{
    mMutex.lock();
    mExit = true;
    mCondition.wakeAll();
    mMutex.unlock();
    wait();
}

void FramelessWorker::postEvent(FramelessEvent *event)
{
    mMutex.lock();
    mEventQueue.append(event);
    mCondition.wakeAll();
    mMutex.unlock();
}

FramelessEvent *FramelessWorker::takeEvent()
{
    QMutexLocker locker(&mMutex);
    while (mEventQueue.isEmpty() && !mExit)
        mCondition.wait(&mMutex);

    FramelessEvent *event = nullptr;
    if (mEventQueue.length() > 0)
        event = mEventQueue.takeFirst();

    return event;
}

void FramelessWorker::run()
{
    while (true) {
        FramelessEvent *event = takeEvent();
        if (mExit) {
            delete event;
            break;
        }

        switch (event->type()) {
        case FramelessEvent::FocusIn:
            focusIn(static_cast<FramelessFocusInEvent *>(event));
            break;

        case FramelessEvent::MouseHover:
            mouseHover(static_cast<FramelessMouseMoveEvent *>(event));
            break;

        case FramelessEvent::MousePress:
            mousePress(static_cast<FramelessMousePressEvent *>(event));
            break;

        case FramelessEvent::MouseMove:
            mouseMove(static_cast<FramelessMouseMoveEvent *>(event));
            break;

        case FramelessEvent::MouseRelease:
            mouseRelease(static_cast<FramelessMouseReleaseEvent *>(event));
            break;

        case FramelessEvent::Leave:
            leave(static_cast<FramelessLeaveEvent *>(event));
            break;

        default:
            break;
        }

        delete event;
    }
}

FramelessWorker::DirAndCursorShape FramelessWorker::calcDirAndCursorShape(const QRect &rOrigin, const QPoint &cursorGlobalPoint, int framelessBorder, qreal margins)
{
    int x = cursorGlobalPoint.x();
    int y = cursorGlobalPoint.y();

    DirAndCursorShape dirAndShape;
    if (rOrigin.x() + framelessBorder + margins > x
            && rOrigin.x() <= x
            && rOrigin.y() + framelessBorder + margins > y
            && rOrigin.y() <= y) {
        dirAndShape.dir = static_cast<int>(Frameless::Direction::TopLeft);
        dirAndShape.cursorShape = Qt::SizeFDiagCursor;
    } else if (x > rOrigin.right() - framelessBorder - margins
              && x <= rOrigin.right()
              && y > rOrigin.bottom() - framelessBorder - margins
              && y <= rOrigin.bottom()) {
        dirAndShape.dir = static_cast<int>(Frameless::Direction::BottomRight);
        dirAndShape.cursorShape = Qt::SizeFDiagCursor;
    } else if (x < rOrigin.x() + framelessBorder + margins
              && x >= rOrigin.x()
              && y > rOrigin.bottom() - framelessBorder - margins
              && y <= rOrigin.bottom()) {
        dirAndShape.dir = static_cast<int>(Frameless::Direction::BottomLeft);
        dirAndShape.cursorShape = Qt::SizeBDiagCursor;
    } else if (x <= rOrigin.right()
              && x >= rOrigin.right() - framelessBorder - margins
              && y >= rOrigin.y()
              && y <= rOrigin.y() + framelessBorder + margins) {
        dirAndShape.dir = static_cast<int>(Frameless::Direction::TopRight);
        dirAndShape.cursorShape = Qt::SizeBDiagCursor;
    } else if (x < rOrigin.x() + framelessBorder + margins && x >= rOrigin.x()) {
        dirAndShape.dir = static_cast<int>(Frameless::Direction::Left);
        dirAndShape.cursorShape = Qt::SizeHorCursor;
    } else if (x <= rOrigin.right() && x > rOrigin.right() - framelessBorder - margins) {
        dirAndShape.dir = static_cast<int>(Frameless::Direction::Right);
        dirAndShape.cursorShape = Qt::SizeHorCursor;
    } else if (y >= rOrigin.y() && y < rOrigin.y() + framelessBorder + margins) {
        dirAndShape.dir = static_cast<int>(Frameless::Direction::Up);
        dirAndShape.cursorShape = Qt::SizeVerCursor;
    } else if (y <= rOrigin.bottom() && y > rOrigin.bottom() - framelessBorder - margins) {
        dirAndShape.dir = static_cast<int>(Frameless::Direction::Down);
        dirAndShape.cursorShape = Qt::SizeVerCursor;
    } else {
        dirAndShape.dir = static_cast<int>(Frameless::Direction::None);
        dirAndShape.cursorShape = Qt::ArrowCursor;
    }

    return dirAndShape;
}

QRect FramelessWorker::calcPositionRect(Frameless::Direction dir, QWindow *target, const QRect &rOrigin, const QPoint &gloPoint)
{
    QRect rMove(rOrigin);

    switch (dir) {
    case Frameless::Direction::Left: {
        if (rOrigin.right() - gloPoint.x() <= target->minimumWidth()) {
            int left = rMove.right() - target->minimumWidth();
            rMove.setX(left > 0 ? left : rMove.right());
            rMove.setWidth(target->minimumWidth());
        } else {
            rMove.setX(gloPoint.x());
        }
    }
        break;

    case Frameless::Direction::Right: {
        if (gloPoint.x() - rOrigin.left() <= target->minimumWidth()) {
            rMove.setX(rOrigin.left());
            rMove.setWidth(target->minimumWidth());
        } else {
            rMove.setX(rOrigin.left());
            rMove.setWidth(gloPoint.x() - rOrigin.left());
        }
    }
        break;

    case Frameless::Direction::Up: {
        if (rOrigin.bottom() - gloPoint.y() <= target->minimumHeight()) {
            int top = rMove.bottom() - target->minimumHeight();
            rMove.setY(top > 0 ? top : rMove.bottom());
            rMove.setHeight(target->minimumHeight());
        } else {
            rMove.setY(gloPoint.y());

        }
    }
        break;

    case Frameless::Direction::Down: {
        if (gloPoint.y() - rOrigin.top() <= target->minimumHeight()) {
            rMove.setY(rOrigin.top());
            rMove.setHeight(target->minimumHeight());
        } else {
            rMove.setY(rOrigin.top());
            rMove.setHeight(gloPoint.y() - rOrigin.top());
        }
    }
        break;

    case Frameless::Direction::TopLeft: {
        if (rOrigin.right() - gloPoint.x() <= target->minimumWidth()) {
            int left = rMove.right() - target->minimumWidth();
            rMove.setX(left > 0 ? left : rMove.right());
            rMove.setWidth(target->minimumWidth());
        } else {
            rMove.setX(gloPoint.x());
        }

        if (rOrigin.bottom() - gloPoint.y() <= target->minimumHeight()) {
            int top = rMove.bottom() - target->minimumHeight();
            rMove.setY(top > 0 ? top : rMove.bottom());
            rMove.setHeight(target->minimumHeight());
        } else {
            rMove.setY(gloPoint.y());
        }
    }
        break;

    case Frameless::Direction::TopRight: {
        if (gloPoint.x() - rOrigin.left() <= target->minimumWidth()) {
            rMove.setX(rOrigin.left());
            rMove.setWidth(target->minimumWidth());
        } else {
            rMove.setX(rOrigin.left());
            rMove.setWidth(gloPoint.x() - rOrigin.left());
        }

        if (rOrigin.bottom() - gloPoint.y() <= target->minimumHeight()) {
            int top = rMove.bottom() - target->minimumHeight();
            rMove.setY(top > 0 ? top : rMove.bottom());
            rMove.setHeight(target->minimumHeight());
        } else {
            rMove.setY(gloPoint.y());
        }
    }
        break;

    case Frameless::Direction::BottomLeft: {
        if (rOrigin.right() - gloPoint.x() <= target->minimumWidth()) {
            int left = rMove.right() - target->minimumWidth();
            rMove.setX(left > 0 ? left : rMove.right());
            rMove.setWidth(target->minimumWidth());
        } else {
            rMove.setX(gloPoint.x());
        }

        if (gloPoint.y() - rOrigin.top() <= target->minimumHeight()) {
            rMove.setY(rOrigin.top());
            rMove.setHeight(target->minimumHeight());
        } else {
            rMove.setY(rOrigin.top());
            rMove.setHeight(gloPoint.y() - rOrigin.top());
        }
    }
        break;

    case Frameless::Direction::BottomRight: {
        if (gloPoint.x() - rOrigin.left() <= target->minimumWidth()) {
            rMove.setX(rOrigin.left());
            rMove.setWidth(target->minimumWidth());
        } else {
            rMove.setX(rOrigin.left());
            rMove.setWidth(gloPoint.x() - rOrigin.left());
        }

        if (gloPoint.y() - rOrigin.top() <= target->minimumHeight()) {
            rMove.setY(rOrigin.top());
            rMove.setHeight(target->minimumHeight());
        } else {
            rMove.setY(rOrigin.top());
            rMove.setHeight(gloPoint.y() - rOrigin.top());
        }
    }
        break;

    default:
        break;
    }

    return rMove;
}

QRect FramelessWorker::calcOriginRect(QWindow *taget)
{
    const QRect &rect = taget->frameGeometry();
    QPoint tl = rect.topLeft();
    QPoint rb = rect.bottomRight();

    return QRect(tl, rb);
}

void FramelessWorker::focusIn(FramelessFocusInEvent *event)
{
    if (!event->frameless)
        return;

    if (!event->canWindowResize
        || event->target->windowStates() & Qt::WindowFullScreen
        || event->target->windowStates() & Qt::WindowMaximized)
        return;

    DirAndCursorShape dirAndShape = calcDirAndCursorShape(calcOriginRect(event->target),
                                                          event->target->cursor().pos(),
                                                          event->frameless->framelessBorder(),
                                                          event->frameless->contentMargins());
    QMetaObject::invokeMethod(event->frameless, "setCursorByFrameless", Qt::QueuedConnection,
                              Q_ARG(int, int(dirAndShape.cursorShape)));
    event->frameless->setDirection(static_cast<Frameless::Direction>(dirAndShape.dir));
}

void FramelessWorker::mouseHover(FramelessMouseMoveEvent *event)
{
    if (!event->frameless)
        return;

    if (event->frameless->leftMouseButtonPressed()
            || !event->canWindowResize
            || event->target->windowStates() & Qt::WindowFullScreen)
        return;

    QWindow *window = event->target;
    if (!window || event->target->windowStates() & Qt::WindowMaximized)
        return;

    QRect rect = calcOriginRect(event->target);
    DirAndCursorShape dirAndShape = calcDirAndCursorShape(rect, event->globalCursorPositon,
                                                          event->frameless->framelessBorder(),
                                                          event->frameless->contentMargins());
    event->frameless->setDirection(static_cast<Frameless::Direction>(dirAndShape.dir));
    QMetaObject::invokeMethod(event->frameless, "setCursorByFrameless", Qt::QueuedConnection,
                              Q_ARG(int, int(dirAndShape.cursorShape)));
}


void FramelessWorker::mousePress(FramelessMousePressEvent *event)
{
    if (!event->frameless)
        return;

    event->frameless->setLeftMouseButtonPressed(true);

    if (event->frameless->direction() == Frameless::Direction::None) {
        event->frameless->setCurrentCanWindowMove(event->canWindowMove);

        if (event->canWindowMove && !(event->target->windowStates() & Qt::WindowFullScreen) && !(event->target->windowStates() & Qt::WindowMaximized)) {
            event->frameless->setDragPosition(event->globalCursorPositon - event->target->frameGeometry().topLeft());
            QMetaObject::invokeMethod(event->frameless, "readyToStartMove", Qt::QueuedConnection,
                                      Q_ARG(int, int(Qt::SizeAllCursor)));
        }
    } else {
        QMetaObject::invokeMethod(event->frameless, "accpetSystemResize", Qt::QueuedConnection);
    }
}

void FramelessWorker::mouseMove(FramelessMouseMoveEvent *event)
{
    if (!event->frameless)
        return;

    if (!event->frameless->leftMouseButtonPressed()) {
        mouseHover(event);
        return;
    }

    if ((event->frameless->direction() == Frameless::Direction::None) && event->frameless->currentCanWindowMove()) {
        if (event->target->windowStates() & Qt::WindowMaximized
            || event->target->windowStates() & Qt::WindowFullScreen
            || event->frameless->acceptSystemMoving()) {
            // event->target->showNormal();

            // double xRatio = event->globalX() * 1.0 / event->target->width();
            // double yRatio = event->globalY() * 1.0 / event->target->height();
            // int widgetX = event->target->width() * xRatio;
            // int widgetY = event->target->height() * yRatio;

            // event->target->move(event->globalX() - widgetX, event->globalY() - widgetY);
            // event->target->layout()->setMargin(10);

            // dragPosition = event->globalPos() - event->target->frameGeometry().topLeft();
            return;
        }

        QMetaObject::invokeMethod(event->frameless, "moveByFrameless", Qt::QueuedConnection,
                                      Q_ARG(QPoint, event->globalCursorPositon - event->frameless->dragPosition()));
        return;
    }


    if (!event->frameless->acceptSystemResize() && event->canWindowResize) {
        if (event->target->windowStates() & Qt::WindowMaximized || event->target->windowStates() & Qt::WindowFullScreen)
            return;

        if (event->frameless->direction() == Frameless::Direction::None)
            return;

        QPoint gloPoint = event->globalCursorPositon;
        const QRect &rect = calcPositionRect(event->frameless->direction(),
                                             event->target, calcOriginRect(event->target), gloPoint);
        QMetaObject::invokeMethod(event->frameless, "setGeometryByFrameless", Qt::QueuedConnection,
                                  Q_ARG(QRect, rect));
    }
}

void FramelessWorker::mouseRelease(FramelessMouseReleaseEvent *event)
{
    if (!event->frameless)
        return;

    QMetaObject::invokeMethod(event->frameless, "unsetCursorByFrameless", Qt::QueuedConnection);
    event->frameless->setLeftMouseButtonPressed(false);
    event->frameless->setDirection(Frameless::Direction::None);
    event->frameless->setDragPosition({0, 0});
}

void FramelessWorker::leave(FramelessLeaveEvent *event)
{
    if (!event->frameless)
        return;

    if (event->frameless->leftMouseButtonPressed())
        return;

    event->frameless->setDirection(Frameless::Direction::None);
    QMetaObject::invokeMethod(event->frameless, "unsetCursorByFrameless", Qt::QueuedConnection);
}
