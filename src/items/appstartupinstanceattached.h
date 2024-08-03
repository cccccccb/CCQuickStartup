#ifndef APPSTARTUPINSTANCEATTACHED_H
#define APPSTARTUPINSTANCEATTACHED_H

#include <QObject>
#include <QtQml>

#include "ccquickstartup_global.h"

class CustomSubItemMetaObject;
class AppStartupInstanceAttachedPrivate;

class CC_QUICKSTARTUP_EXPORT AppStartupInstanceAttached : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool loaded READ loaded NOTIFY loadedChanged FINAL)

    QML_UNCREATABLE("AppStartupInstanceAttached Attached.")
    QML_NAMED_ELEMENT(StartupItem)
    QML_ATTACHED(AppStartupInstanceAttached)

    friend class CustomSubItemMetaObject;
    friend class AppQmlComponentIncubator;

public:
    explicit AppStartupInstanceAttached(QObject *parent = nullptr);
    ~AppStartupInstanceAttached();

    bool loaded() const;
    void setLoaded(bool loaded);

    static AppStartupInstanceAttached *qmlAttachedProperties(QObject *object);

Q_SIGNALS:
    void loadedChanged();

private:
    void insertSubObject(const QString &name, QObject *item);

private:
    QScopedPointer<AppStartupInstanceAttachedPrivate> dd;
};

QML_DECLARE_TYPEINFO(AppStartupInstanceAttached, QML_HAS_ATTACHED_PROPERTIES)

#endif // APPSTARTUPINSTANCEATTACHED_H
