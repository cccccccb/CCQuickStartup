#ifndef CCSTARTUPPRELOADINTERFACE_H
#define CCSTARTUPPRELOADINTERFACE_H

#include <QSGRendererInterface>
#include <QtPlugin>

#include "ccquickstartup_global.h"

QT_BEGIN_NAMESPACE
class QQmlApplicationEngine;
class QQmlEngine;
QT_END_NAMESPACE

class CC_QUICKSTARTUP_EXPORT CCStartupPreloadInterface
{
public:
    CCStartupPreloadInterface() = default;
    virtual ~CCStartupPreloadInterface() = default;

    virtual QUrl preloadComponentPath() const = 0;

    virtual void aboutToPreload(QQmlApplicationEngine *engine);
    virtual void preloadCreated(QQmlApplicationEngine *engine);
};

#define CCStartupPreloadInterface_iid "org.orange.startup.preload.interface"
Q_DECLARE_INTERFACE(CCStartupPreloadInterface, CCStartupPreloadInterface_iid)

#endif // QUICKSTARTUPPRELOADINTERFACE_H
