#ifndef FRAMELESS_H
#define FRAMELESS_H

#include <QEvent>
#include <QMargins>
#include <QObject>
#include <QPoint>
#include <QRect>
#include <QString>

class QEvent;
class QWindow;
class QMouseEvent;
class QFocusEvent;
class FramelessWorker;

class Frameless : public QObject
{
    Q_OBJECT

public:
    explicit Frameless(QWindow *self, QObject *parent = nullptr);

    void setEnabled(bool enabled);
    bool enabled() const;

    void setCanWindowMove(bool canMove);
    bool canWindowMove() const;

    void setCanWindowResize(bool canResize);
    bool canWindowResize() const;

    void setFramelessBorder(int framelessBorder);
    int framelessBorder() const;

    void setContentMargins(qreal newContentMargins);
    qreal contentMargins() const;

    enum class Direction {
        None = -1,
        Up,
        Down,
        Left,
        Right,
        TopLeft,
        TopRight,
        BottomLeft,
        BottomRight
    };

    void setDirection(Direction dir);
    Direction direction() const;

    void setCurrentCanWindowMove(bool canWindowMove);
    bool currentCanWindowMove() const;

    void setDragPosition(const QPoint &dragPosition);
    QPoint dragPosition() const;

    void setLeftMouseButtonPressed(bool pressed);
    bool leftMouseButtonPressed() const;

    void setAcceptSystemResize(bool accept);
    bool acceptSystemResize() const;

    void setAcceptSystemMoving(bool accept);
    bool acceptSystemMoving() const;

    bool framelessMoving() const;

    void targetEvent(QEvent *event);

    QWindow *window() const;

    Q_INVOKABLE void moveByFrameless(const QPoint &pos);
    Q_INVOKABLE void setGeometryByFrameless(const QRect &rect);
    Q_INVOKABLE void setCursorByFrameless(int shape);
    Q_INVOKABLE void unsetCursorByFrameless();
    Q_INVOKABLE void readyToStartMove(int shape);
    Q_INVOKABLE void accpetSystemResize();

private:
    // worker and target
    QWindow *           mSelf;
    FramelessWorker *   mWorker;
    bool                mEnabled = false;
    bool                mCanWindowMove = false;
    bool                mCanWindowResize = false;

    // state for window
    QPoint              mDragPosition;
    Direction           mDir;
    bool                mLeftButtonPress = false;
    bool                mAlreadyChangeCursor = false;
    bool                mCurrentCanWindowMove = false;
    int                 mOverrideCursorShape = false;
    bool                mAcceptSystemResize = false;
    bool                mAcceptSystemMoving = false;
    int                 m_frameBorder       = 0;
    qreal               m_contentMargins    = 0.0;
};

#endif // FRAMELESS_H
