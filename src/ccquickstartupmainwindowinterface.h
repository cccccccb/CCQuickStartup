#ifndef CCQUICKSTARTUPMAINWINDOWINTERFACE_H
#define CCQUICKSTARTUPMAINWINDOWINTERFACE_H

#include <QGuiApplication>
#include <QtPlugin>

#include "ccquickstartup_global.h"

QT_BEGIN_NAMESPACE
class QQmlApplicationEngine;
QT_END_NAMESPACE

class CC_QUICKSTARTUP_EXPORT CCQuickStartupMainWindowInterface
{
public:
    CCQuickStartupMainWindowInterface() = default;
    virtual ~CCQuickStartupMainWindowInterface() = default;

    virtual QUrl mainComponentPath() const = 0;
    virtual void initialize(QQmlApplicationEngine *engine);
    virtual void finishedLoading(QQmlApplicationEngine *engine);
};

#define CCQuickStartupMainWindowInterface_iid "org.orange.app.mainwindow.interface"
Q_DECLARE_INTERFACE(CCQuickStartupMainWindowInterface, CCQuickStartupMainWindowInterface_iid)
#endif // DQMLAPPPLUGININTERFACE_H
