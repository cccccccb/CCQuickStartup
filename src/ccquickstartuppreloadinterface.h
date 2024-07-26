#ifndef CCQUICKSTARTUPPRELOADINTERFACE_H
#define CCQUICKSTARTUPPRELOADINTERFACE_H

#include <QSGRendererInterface>
#include <QtPlugin>

#include "ccquickstartup_global.h"

QT_BEGIN_NAMESPACE
class QQmlApplicationEngine;
class QQmlEngine;
QT_END_NAMESPACE

class CC_QUICKSTARTUP_EXPORT CCQuickStartupPreloadInterface
{
public:
    CCQuickStartupPreloadInterface() = default;
    virtual ~CCQuickStartupPreloadInterface() = default;

    virtual QUrl preloadComponentPath() const = 0;

    virtual void aboutToPreload(QQmlApplicationEngine *engine);
    virtual void preloadCreated(QQmlApplicationEngine *engine);
};

#define CCQuickStartupPreloadInterface_iid "org.orange.app.preload.interface"
Q_DECLARE_INTERFACE(CCQuickStartupPreloadInterface, CCQuickStartupPreloadInterface_iid)

#endif // QUICKSTARTUPPRELOADINTERFACE_H
