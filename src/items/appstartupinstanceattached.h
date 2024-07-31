#ifndef APPSTARTUPINSTANCEATTACHED_H
#define APPSTARTUPINSTANCEATTACHED_H

#include <QObject>
#include <QQmlComponent>

#include "ccquickstartup_global.h"

QT_BEGIN_NAMESPACE
class QQuickTransition;
QT_END_NAMESPACE

class CustomSubItemMetaObject;
class AppStartupInstanceAttachedPrivate;

class CC_QUICKSTARTUP_EXPORT AppStartupInstanceAttached : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QQuickTransition *displayPopulate READ displayPopulate WRITE setDisplayPopulate NOTIFY displayPopulateChanged FINAL)
    Q_PROPERTY(bool loaded READ loaded NOTIFY loadedChanged FINAL)

    QML_ANONYMOUS
    QML_UNCREATABLE("AppStartupInstanceAttached Attached.")
    QML_NAMED_ELEMENT(StartupItem)
    QML_ATTACHED(AppStartupInstanceAttached)

    friend class CustomSubItemMetaObject;
    friend class AppQmlComponentIncubator;

public:
    explicit AppStartupInstanceAttached(QObject *parent = nullptr);
    ~AppStartupInstanceAttached();

    QQuickTransition *displayPopulate() const;
    void setDisplayPopulate(QQuickTransition *transition);

    bool loaded() const;
    void setLoaded(bool loaded);

    static AppStartupInstanceAttached *qmlAttachedProperties(QObject *object);

Q_SIGNALS:
    void displayPopulateChanged();
    void loadedChanged();

private:
    void setSubObject(const QByteArray &name, QObject *item);

private:
    QScopedPointer<AppStartupInstanceAttachedPrivate> dd;
};

QML_DECLARE_TYPEINFO(AppStartupInstanceAttached, QML_HAS_ATTACHED_PROPERTIES)

#endif // APPSTARTUPINSTANCEATTACHED_H
