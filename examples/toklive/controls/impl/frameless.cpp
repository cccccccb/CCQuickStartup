#include "frameless.h"
#include "framelessworker.h"
#include "framelessworkerevent.h"

#include <QMutexLocker>
#include <QWindow>
#include <QMouseEvent>
#include <QRect>
#include <QDebug>
#include <QTimer>
#include <QDateTime>
#include <QGuiApplication>

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
    if (mEnabled == enabled)
        return;

    mEnabled = enabled;

    if (enabled) {
        mSelf->setFlags(mSelf->flags() | Qt::FramelessWindowHint | Qt::WindowSystemMenuHint);
        if (!mWorker->isRunning())
            mWorker->start();
    } else {
        mSelf->setFlag(Qt::FramelessWindowHint, false);
        mSelf->setFlag(Qt::WindowSystemMenuHint, false);
    }
}

bool Frameless::enabled() const
{
    return mEnabled;
}

void Frameless::setDirection(Direction dir)
{
    QMutexLocker locker(&m_mutex);
    mDir = dir;
}

Frameless::Direction Frameless::direction() const
{
    QMutexLocker locker(&m_mutex);
    return mDir;
}

void Frameless::setCurrentCanWindowMove(bool canWindowMove)
{
    QMutexLocker locker(&m_mutex);
    mCurrentCanWindowMove = canWindowMove;
}

bool Frameless::currentCanWindowMove() const
{
    QMutexLocker locker(&m_mutex);
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
    if (framelessBorder < 0)
        framelessBorder = 0;
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
    QMutexLocker locker(&m_mutex);
    mDragPosition = dragPosition;
}

QPoint Frameless::dragPosition() const
{
    QMutexLocker locker(&m_mutex);
    return mDragPosition;
}

void Frameless::setLeftMouseButtonPressed(bool pressed)
{
    QMutexLocker locker(&m_mutex);
    mLeftButtonPress = pressed;
}

bool Frameless::leftMouseButtonPressed() const
{
    QMutexLocker locker(&m_mutex);
    return mLeftButtonPress;
}

void Frameless::setAcceptSystemResize(bool accept)
{
    QMutexLocker locker(&m_mutex);
    mAcceptSystemResize = accept;
}

bool Frameless::acceptSystemResize() const
{
    QMutexLocker locker(&m_mutex);
    return mAcceptSystemResize;
}

void Frameless::setAcceptSystemMoving(bool accept)
{
    QMutexLocker locker(&m_mutex);
    mAcceptSystemMoving = accept;
}

bool Frameless::acceptSystemMoving() const
{
    QMutexLocker locker(&m_mutex);
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
    case Frameless::Direction::None:
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

