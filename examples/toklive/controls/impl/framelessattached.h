#ifndef FRAMELESSATTACHED_H
#define FRAMELESSATTACHED_H

#include <QObject>
#include <QQuickWindow>
#include <QQuickItem>
#include <QQmlListProperty>

class Frameless;
class FramelessAttached : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged FINAL)
    Q_PROPERTY(QQmlListProperty<QQuickItem> moveUnder READ moveUnder CONSTANT)
    Q_PROPERTY(QQmlListProperty<QQuickItem> moveExclude READ moveExclude CONSTANT)
    Q_PROPERTY(int frameBorder READ frameBorder WRITE setFrameBorder NOTIFY frameBorderChanged FINAL)
    Q_PROPERTY(bool canWindowResize READ canWindowResize WRITE setCanWindowResize NOTIFY canWindowResizeChanged FINAL)
    Q_PROPERTY(qreal contentMargins READ contentMargins WRITE setContentMargins NOTIFY contentMarginsChanged FINAL)
    QML_ANONYMOUS


#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    QML_UNCREATABLE("FramelessAttached Attached.")
    QML_NAMED_ELEMENT(FramelessAttached)
    QML_ATTACHED(FramelessAttached)
#endif

public:
    explicit FramelessAttached(QQuickWindow *parent);

    QQmlListProperty<QQuickItem> moveUnder();
    QQmlListProperty<QQuickItem> moveExclude();

    int frameBorder() const;
    void setFrameBorder(int newFrameBorder);

    bool canWindowResize() const;
    void setCanWindowResize(bool newCanWindowResize);

    bool enabled() const;
    void setEnabled(bool newEnable);

    qreal contentMargins() const;
    void setContentMargins(qreal newContentMargins);

    Q_INVOKABLE void activateWindow();

    static FramelessAttached *qmlAttachedProperties(QObject *object);

Q_SIGNALS:
    void frameBorderChanged();
    void canWindowResizeChanged();
    void enabledChanged();
    void contentMarginsChanged();

private:
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    Frameless *m_frameless = nullptr;
    QVector<QQuickItem *> m_moveUnderContainer;
    QVector<QQuickItem *> m_moveExcludeContainer;
};

QML_DECLARE_TYPEINFO(FramelessAttached, QML_HAS_ATTACHED_PROPERTIES)
#endif // FRAMELESSATTACHED_H
