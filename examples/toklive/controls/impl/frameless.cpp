#include "Frameless.h"
#include "FramelessWorker.h"
#include "FramelessWorkerEvent.h"

#include <QWindow>
#include <QMouseEvent>
#include <QRect>
#include <QDebug>
#include <QTimer>
#include <QDateTime>

#ifdef Q_OS_WINDOWS
#include <dwmapi.h>
#else
#endif

#define FREMELESS_BORDER 6

Frameless::Frameless(QWindow *self, QObject *parent)
    : QObject(parent)
    , mSelf(self)
    , mWorker(FramelessWorker::instance())
    , mCanWindowMove(false)
    , mCanWindowResize(false)
    , mDir(Direction::None)
    , mLeftButtonPress(false)
    , mAlreadyChangeCursor(false)
    , mCurrentCanWindowMove(false)
    , mOverrideCursorShape(Qt::ArrowCursor)
    , m_frameBorder(FREMELESS_BORDER)
{

}

void Frameless::setEnabled(bool enabled)
{
    if (enabled) {
        if (mWorker->isRunning())
            return;

        mSelf->setFlags(mSelf->flags() | Qt::FramelessWindowHint | Qt::WindowSystemMenuHint);
        mWorker->start();
    } else {
        if (!mWorker->isRunning())
            return;

        mSelf->setFlag(Qt::FramelessWindowHint, false);
        mSelf->setFlag(Qt::WindowSystemMenuHint, false);
        mWorker->quit();
    }
}

bool Frameless::enabled() const
{
    return mWorker->isRunning();
}

void Frameless::setDirection(Direction dir)
{
    mDir = dir;
}

Frameless::Direction Frameless::direction() const
{
    return mDir;
}

void Frameless::setCurrentCanWindowMove(bool canWindowMove)
{
    mCurrentCanWindowMove = canWindowMove;
}

bool Frameless::currentCanWindowMove() const
{
    return mCurrentCanWindowMove;
}

void Frameless::setCanWindowMove(bool canMove)
{
    mCanWindowMove = canMove;
}

bool Frameless::canWindowMove() const
{
    return mCanWindowMove;
}

void Frameless::setCanWindowResize(bool canResize)
{
    if (mCanWindowResize == canResize)
        return;

    mCanWindowResize = canResize;
}

bool Frameless::canWindowResize() const
{
    return mCanWindowResize;
}

void Frameless::setFramelessBorder(int framelessBorder)
{
    if (m_frameBorder == framelessBorder)
        return;

    m_frameBorder = framelessBorder;
}

int Frameless::framelessBorder() const
{
    return m_frameBorder;
}

void Frameless::setContentMargins(qreal newContentMargins)
{
    if (m_contentMargins == newContentMargins)
        return;

    m_contentMargins = newContentMargins;
}

qreal Frameless::contentMargins() const
{
    return m_contentMargins;
}

void Frameless::setDragPosition(const QPoint &dragPosition)
{
    mDragPosition = dragPosition;
}

QPoint Frameless::dragPosition() const
{
    return mDragPosition;
}

void Frameless::setLeftMouseButtonPressed(bool pressed)
{
    mLeftButtonPress = pressed;
}

bool Frameless::leftMouseButtonPressed() const
{
    return mLeftButtonPress;
}

void Frameless::setAcceptSystemResize(bool accept)
{
    mAcceptSystemResize = accept;
}

bool Frameless::acceptSystemResize() const
{
    return mAcceptSystemResize;
}

void Frameless::setAcceptSystemMoving(bool accept)
{
    mAcceptSystemMoving = accept;
}

bool Frameless::acceptSystemMoving() const
{
    return mAcceptSystemMoving;
}

void Frameless::targetEvent(QEvent *event)
{
    FramelessEvent *framelessEvent = nullptr;
    switch (event->type()) {
    case QEvent::MouseButtonPress: {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
        if (mouseEvent->button() != Qt::LeftButton)
            break;

        FramelessMousePressEvent *mousePressEvent = new FramelessMousePressEvent();
        mousePressEvent->globalCursorPositon = mouseEvent->globalPosition().toPoint();
        mousePressEvent->canWindowMove = mCanWindowMove;
        framelessEvent = mousePressEvent;
    }
        break;
    case QEvent::NonClientAreaMouseButtonRelease:
    case QEvent::MouseButtonRelease: {
        FramelessMouseReleaseEvent *mouseReleaseEvent = new FramelessMouseReleaseEvent();
        framelessEvent = mouseReleaseEvent;
    }
        break;
    case QEvent::MouseMove: {
        FramelessMouseMoveEvent *mouseMoveEvent = new FramelessMouseMoveEvent();
        mouseMoveEvent->canWindowResize = mCanWindowResize;
        mouseMoveEvent->globalCursorPositon = static_cast<QMouseEvent *>(event)->globalPosition().toPoint();

        framelessEvent = mouseMoveEvent;
    }
        break;
    case QEvent::Leave: {
        FramelessLeaveEvent *leave = new FramelessLeaveEvent();
        framelessEvent = leave;
    }
        break;
    case QEvent::FocusIn: {
        FramelessFocusInEvent *focusInEvent = new FramelessFocusInEvent();
        focusInEvent->canWindowResize = mCanWindowResize;
        framelessEvent = focusInEvent;
    }
        break;
    default:
        break;
    }

    if (framelessEvent) {
        framelessEvent->target = mSelf;
        framelessEvent->frameless = this;

        mWorker->postEvent(framelessEvent);
    }
}

QWindow *Frameless::window() const
{
    return mSelf;
}

void Frameless::moveByFrameless(const QPoint &pos)
{
    mSelf->setGeometry(QRect(pos, QSize(mSelf->width(), mSelf->height())));
}

void Frameless::setGeometryByFrameless(const QRect &rect)
{
    mSelf->setGeometry(rect);
}

void Frameless::setCursorByFrameless(int shape)
{
    if (mAlreadyChangeCursor) {
        if (shape == Qt::ArrowCursor) {
            unsetCursorByFrameless();
            return;
        }

        qApp->changeOverrideCursor(Qt::CursorShape(shape));
    } else {
        if (shape == Qt::ArrowCursor)
            return;

        qApp->setOverrideCursor(Qt::CursorShape(shape));
        mAlreadyChangeCursor = true;
    }

    mOverrideCursorShape = shape;
}

void Frameless::unsetCursorByFrameless()
{
    if (!mAlreadyChangeCursor)
        return;

    mAlreadyChangeCursor = false;

    QCursor *cursor = qApp->overrideCursor();
    if (!cursor || cursor->shape() != mOverrideCursorShape)
        return;

    qApp->restoreOverrideCursor();
}


void Frameless::readyToStartMove(int shape)
{
    setAcceptSystemMoving(window()->startSystemMove());
    // setCursorByFrameless(shape);
}

static Qt::Edges directionToQtEdges(Frameless::Direction dir)
{
    switch (dir) {
    case Frameless::Direction::Up:
        return Qt::TopEdge;
    case Frameless::Direction::Down:
        return Qt::BottomEdge;
    case Frameless::Direction::Left:
        return Qt::LeftEdge;
    case Frameless::Direction::Right:
        return Qt::RightEdge;
    case Frameless::Direction::TopLeft:
        return Qt::LeftEdge | Qt::TopEdge;
    case Frameless::Direction::TopRight:
        return Qt::RightEdge | Qt::TopEdge;
    case Frameless::Direction::BottomLeft:
        return Qt::LeftEdge | Qt::BottomEdge;
    case Frameless::Direction::BottomRight:
        return Qt::RightEdge | Qt::BottomEdge;
    default:
        break;
    }

    return Qt::TopEdge;
}

void Frameless::accpetSystemResize()
{
    if (direction() == Frameless::Direction::None)
        return;

    setAcceptSystemResize(window()->startSystemResize(directionToQtEdges(direction())));
}

