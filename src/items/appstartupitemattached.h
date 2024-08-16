#ifndef APPSTARTUPITEMATTACHED_H
#define APPSTARTUPITEMATTACHED_H

#include <QQmlPropertyMap>
#include <QtQml>

#include "ccquickstartup_global.h"

QT_BEGIN_NAMESPACE
class QQuickItem;
QT_END_NAMESPACE

class AppStartupItem;
class AppStartupComponentGroup;
class AppStartupItemAttachedPrivate;

class CC_QUICKSTARTUP_EXPORT AppStartupItemAttached : public QQmlPropertyMap
{
    Q_OBJECT
    Q_PROPERTY(bool loaded READ loaded NOTIFY loadedChanged FINAL)
    Q_PROPERTY(AppStartupItem *startupItem READ startupItem NOTIFY startupItemChanged FINAL)

    QML_UNCREATABLE("AppStartupItemAttached Attached.")
    QML_ANONYMOUS

public:
    explicit AppStartupItemAttached(QObject *parent = nullptr);
    ~AppStartupItemAttached();

    bool loaded() const;
    void setLoaded(bool loaded);

    AppStartupItem *startupItem() const;
    void setStartupItem(AppStartupItem *item);

    Q_INVOKABLE void loadComponent(const AppStartupComponentGroup &information, QQuickItem *container);
    Q_INVOKABLE void unloadComponent(const AppStartupComponentGroup &information);
    Q_INVOKABLE QQuickItem *componentContainer(const AppStartupComponentGroup &information) const;

Q_SIGNALS:
    void loadedChanged();
    void startupItemChanged();

private:
    QScopedPointer<AppStartupItemAttachedPrivate> dd;
};

#endif // APPSTARTUPITEMATTACHED_H
