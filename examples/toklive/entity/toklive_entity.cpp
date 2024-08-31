#include "toklive_entity.h"
#include "appstartupinstance.h"

#include <QIcon>
#include <QQmlApplicationEngine>
#include <QTimer>

TokLiveEntity::TokLiveEntity(QObject *parent)
    : QObject{parent}
{}

TokLiveEntity::~TokLiveEntity()
{

}

QUrl TokLiveEntity::entityModulePath() const
{
    return QUrl("qrc:///src/MainWindow.qml");
}

void TokLiveEntity::initialize(QQmlApplicationEngine *)
{
    QTimer::singleShot(20000, []() {
        AppStartupInstance::instance()->reload();
    });
    QIcon::setThemeName("TokLive-desktop");
}
