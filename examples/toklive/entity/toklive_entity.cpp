#include "toklive_entity.h"
#include "appstartupinstance.h"

#include <QIcon>
#include <QQmlApplicationEngine>

TokLiveEntity::TokLiveEntity(QObject *parent)
    : QObject{parent}
{}

TokLiveEntity::~TokLiveEntity()
{

}

QUrl TokLiveEntity::entityComponentPath() const
{
    return QUrl("qrc:///src/MainWindow.qml");
}

void TokLiveEntity::initialize(QQmlApplicationEngine *)
{
    QIcon::setThemeName("TokLive-desktop");
}
