#ifndef TOKLIVEMAINWINDOW_H
#define TOKLIVEMAINWINDOW_H

#include <QObject>

#include "ccquickstartupmainwindowinterface.h"

class TokLiveQmlInstance;

class TokLiveMainWindow : public QObject, public CCQuickStartupMainWindowInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID CCQuickStartupMainWindowInterface_iid FILE "plugin.json")
    Q_INTERFACES(CCQuickStartupMainWindowInterface)

public:
    explicit TokLiveMainWindow(QObject *parent = nullptr);
    ~TokLiveMainWindow();

    QUrl mainComponentPath() const;
    void initialize(QQmlApplicationEngine *engine);
};

#endif // TOKLIVEMAINWINDOW_H
