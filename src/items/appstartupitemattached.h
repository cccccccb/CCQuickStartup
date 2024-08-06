#ifndef APPSTARTUPITEMATTACHED_H
#define APPSTARTUPITEMATTACHED_H

#include <QQmlPropertyMap>
#include <QtQml>

#include "ccquickstartup_global.h"

class CustomSubItemMetaObject;
class AppStartupItemAttachedPrivate;

class CC_QUICKSTARTUP_EXPORT AppStartupItemAttached : public QQmlPropertyMap
{
    Q_OBJECT
    Q_PROPERTY(bool loaded READ loaded NOTIFY loadedChanged FINAL)

    QML_UNCREATABLE("AppStartupItemAttached Attached.")
    QML_NAMED_ELEMENT(AppStartupItem)
    QML_ATTACHED(AppStartupItemAttached)

    friend class AppQmlComponentIncubator;

public:
    explicit AppStartupItemAttached(QObject *parent = nullptr);
    ~AppStartupItemAttached();

    bool loaded() const;
    void setLoaded(bool loaded);

Q_SIGNALS:
    void loadedChanged();

private:
    QScopedPointer<AppStartupItemAttachedPrivate> dd;
};

#endif // APPSTARTUPITEMATTACHED_H
