#ifndef CCSTARTUPMAINWINDOWINTERFACE_H
#define CCSTARTUPMAINWINDOWINTERFACE_H

#include <QGuiApplication>
#include <QtPlugin>

#include "ccquickstartup_global.h"

QT_BEGIN_NAMESPACE
class QQmlApplicationEngine;
QT_END_NAMESPACE

class CC_QUICKSTARTUP_EXPORT CCStartupMainWindowInterface
{
public:
    CCStartupMainWindowInterface() = default;
    virtual ~CCStartupMainWindowInterface() = default;

    virtual QUrl mainComponentPath() const = 0;
    virtual void initialize(QQmlApplicationEngine *engine);
    virtual void finishedLoading(QQmlApplicationEngine *engine);
};

#define CCStartupMainWindowInterface_iid "org.orange.startup.mainwindow.interface"
Q_DECLARE_INTERFACE(CCStartupMainWindowInterface, CCStartupMainWindowInterface_iid)
#endif // DQMLAPPPLUGININTERFACE_H
