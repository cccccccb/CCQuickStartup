#include "toklivemainwindow.h"
#include "appstartupinstance.h"

#include <QIcon>
#include <QQmlApplicationEngine>

TokLiveMainWindow::TokLiveMainWindow(QObject *parent)
    : QObject{parent}
{}

TokLiveMainWindow::~TokLiveMainWindow()
{

}

QUrl TokLiveMainWindow::mainComponentPath() const
{
    return QUrl("qrc:///src/MainWindow.qml");
}

void TokLiveMainWindow::initialize(QQmlApplicationEngine *)
{
    QIcon::setThemeName("TokLive-desktop");
}
