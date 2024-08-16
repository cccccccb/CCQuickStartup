#ifndef APPSTARTUPAPPLICATIONFACTORY_H
#define APPSTARTUPAPPLICATIONFACTORY_H

#include "ccquickstartup_global.h"

QT_BEGIN_NAMESPACE
class QGuiApplication;
class QQmlApplicationEngine;
QT_END_NAMESPACE

class CC_QUICKSTARTUP_EXPORT AppStartupApplicationFactory
{
public:
    AppStartupApplicationFactory() = default;
    virtual ~AppStartupApplicationFactory() {}

    virtual QGuiApplication *createApplication(int &argc, char **argv) = 0;
    virtual QQmlApplicationEngine *createEngine() = 0;
};

#endif // APPSTARTUPAPPLICATIONFACTORY_H
