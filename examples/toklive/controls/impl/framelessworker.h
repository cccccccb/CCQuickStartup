#ifndef FRAMELESSWORKER_H
#define FRAMELESSWORKER_H

#include "Frameless.h"

#include <QMutex>
#include <QThread>
#include <QWaitCondition>

class QWindow;
class FramelessEvent;
class FramelessLeaveEvent;
class FramelessFocusInEvent;
class FramelessMouseMoveEvent;
class FramelessMousePressEvent;
class FramelessMouseReleaseEvent;

class  FramelessWorker : public QThread
{
    Q_OBJECT
public:
    static FramelessWorker *instance();
    void postEvent(FramelessEvent *event);

public Q_SLOTS:
    void exit();

protected:
    FramelessEvent *takeEvent();
    void run() override;

    void focusIn(FramelessFocusInEvent *event);
    void mouseHover(FramelessMouseMoveEvent *event);
    void mousePress(FramelessMousePressEvent *event);
    void mouseMove(FramelessMouseMoveEvent *event);
    void mouseRelease(FramelessMouseReleaseEvent *event);
    void leave(FramelessLeaveEvent *event);

    struct DirAndCursorShape
    {
        int dir = -1;
        Qt::CursorShape cursorShape = Qt::ArrowCursor;
    };
    static DirAndCursorShape calcDirAndCursorShape(const QRect &rOrigin, const QPoint &cursorGlobalPoint, int framelessBorder, qreal margins);
    static QRect calcPositionRect(Frameless::Direction dir, QWindow *target, const QRect &rOrigin, const QPoint &gloPoint);
    static QRect calcOriginRect(QWindow *taget);

private:
    explicit FramelessWorker(QObject *parent = nullptr);

private:
    static FramelessWorker *    mInstance;
    bool                        mExit = false;
    QList<FramelessEvent *>     mEventQueue;
    QWaitCondition              mCondition;
    QMutex                      mMutex;
};

#endif // FRAMELESSWORKER_H
