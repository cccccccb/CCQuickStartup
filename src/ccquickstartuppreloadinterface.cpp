#include "ccquickstartuppreloadinterface.h"

#include <QGuiApplication>


void CCQuickStartupPreloadInterface::aboutToPreload(QQmlApplicationEngine *engine)
{
    Q_UNUSED(engine);
}

void CCQuickStartupPreloadInterface::preloadCreated(QQmlApplicationEngine *engine)
{
    Q_UNUSED(engine);
}
