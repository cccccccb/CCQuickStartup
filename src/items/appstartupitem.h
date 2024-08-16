#ifndef DQUICKAppStartupITEM_H
#define DQUICKAppStartupITEM_H

#include <QQuickItem>

#include "ccquickstartup_global.h"

QT_BEGIN_NAMESPACE
class QQmlComponent;
class QQuickItem;
QT_END_NAMESPACE

class AppStartupItemPrivate;
class AppStartupItemAttached;

class CC_QUICKSTARTUP_EXPORT AppStartupItem : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QQuickItem *container READ container NOTIFY containerChanged)
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

    QQuickItem *container() const;
    void setContainer(QQuickItem *w);

    bool asynchronous() const;
    void setAsynchronous(bool a);

    bool populate() const;
    void setPopulate(bool populate);

    static AppStartupItemAttached *qmlAttachedProperties(QObject *object);

Q_SIGNALS:
    void loadedChanged();
    void progressChanged();
    void containerChanged();
    void asynchronousChanged();
    void populateChanged();

private:
    QScopedPointer<AppStartupItemPrivate> dd;
    bool m_populate;
};

QML_DECLARE_TYPEINFO(AppStartupItem, QML_HAS_ATTACHED_PROPERTIES)
#endif  // DQUICKAppStartupITEM_H
