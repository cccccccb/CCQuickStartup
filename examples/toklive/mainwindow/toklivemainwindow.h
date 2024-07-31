#ifndef TOKLIVEMAINWINDOW_H
#define TOKLIVEMAINWINDOW_H

#include <QObject>

#include "ccstartupmainwindowinterface.h"

class TokLiveQmlInstance;

class TokLiveMainWindow : public QObject, public CCStartupMainWindowInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID CCStartupMainWindowInterface_iid FILE "plugin.json")
    Q_INTERFACES(CCStartupMainWindowInterface)

public:
    explicit TokLiveMainWindow(QObject *parent = nullptr);
    ~TokLiveMainWindow();

    QUrl mainComponentPath() const;
    void initialize(QQmlApplicationEngine *engine);
};

#endif // TOKLIVEMAINWINDOW_H
