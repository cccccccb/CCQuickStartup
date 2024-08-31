#ifndef APPSTARTUPENTITYINTERFACE_H
#define APPSTARTUPENTITYINTERFACE_H

#include <QGuiApplication>
#include <QtPlugin>

#include "ccquickstartup_global.h"

QT_BEGIN_NAMESPACE
class QQmlApplicationEngine;
QT_END_NAMESPACE

class CC_QUICKSTARTUP_EXPORT AppStartupEntityInterface
{
public:
    AppStartupEntityInterface() = default;
    virtual ~AppStartupEntityInterface() = default;

    virtual QUrl entityModulePath() const = 0;
    virtual void initialize(QQmlApplicationEngine *engine);
    virtual void finishedLoading(QQmlApplicationEngine *engine);
};

#define AppStartupEntityInterface_iid "org.orange.startup.entity.interface"
Q_DECLARE_INTERFACE(AppStartupEntityInterface, AppStartupEntityInterface_iid)

#endif // APPSTARTUPENTITYINTERFACE_H
