#ifndef APPSTARTUPPRELOADINTERFACE_H
#define APPSTARTUPPRELOADINTERFACE_H

#include <QSGRendererInterface>
#include <QtPlugin>

#include "ccquickstartup_global.h"

QT_BEGIN_NAMESPACE
class QQmlApplicationEngine;
class QQmlEngine;
QT_END_NAMESPACE

class CC_QUICKSTARTUP_EXPORT AppStartupPreloadInterface
{
public:
    AppStartupPreloadInterface() = default;
    virtual ~AppStartupPreloadInterface() = default;

    virtual QUrl preloadComponentPath() const = 0;

    virtual void aboutToPreload(QQmlApplicationEngine *engine);
    virtual void preloadCreated(QQmlApplicationEngine *engine);
};

#define AppStartupPreloadInterface_iid "org.orange.startup.preload.interface"
Q_DECLARE_INTERFACE(AppStartupPreloadInterface, AppStartupPreloadInterface_iid)

#endif // APPSTARTUPPRELOADINTERFACE_H
