#ifndef DQUICKAppStartupITEM_H
#define DQUICKAppStartupITEM_H

#include <QQuickItem>
#include <QQuickWindow>

#include "ccquickstartup_global.h"

QT_BEGIN_NAMESPACE
class QQmlComponent;
class QQuickWindow;
QT_END_NAMESPACE

class AppStartupItemPrivate;
class AppStartupItemAttached;

class CC_QUICKSTARTUP_EXPORT AppStartupItem : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QQuickWindow *containWindow READ containWindow NOTIFY containWindowChanged)
    Q_PROPERTY(bool loaded READ loaded NOTIFY loadedChanged)
    Q_PROPERTY(bool populate READ populate NOTIFY populateChanged)
    Q_PROPERTY(qreal progress READ progress NOTIFY progressChanged)
    Q_PROPERTY(bool asynchronous READ asynchronous WRITE setAsynchronous NOTIFY asynchronousChanged)

    QML_NAMED_ELEMENT(AppStartupItem)
    QML_ATTACHED(AppStartupItemAttached)

public:
    explicit AppStartupItem(QQuickItem *parentItem = nullptr);
    ~AppStartupItem();

    bool loaded() const;
    void setLoaded(bool loaded);

    qreal progress() const;
    void setProgress(qreal progress);

    QQuickWindow *containWindow() const;
    void setContainWindow(QQuickWindow *w);

    bool asynchronous() const;
    void setAsynchronous(bool a);

    bool populate() const;
    void setPopulate(bool populate);

    static AppStartupItemAttached *qmlAttachedProperties(QObject *object);

Q_SIGNALS:
    void loadedChanged();
    void progressChanged();
    void containWindowChanged();
    void asynchronousChanged();
    void populateChanged();

private:
    QScopedPointer<AppStartupItemPrivate> dd;
    bool m_populate;
};

QML_DECLARE_TYPEINFO(AppStartupItemAttached, QML_HAS_ATTACHED_PROPERTIES)
#endif  // DQUICKAppStartupITEM_H
