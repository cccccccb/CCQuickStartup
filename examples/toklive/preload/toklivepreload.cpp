#include "toklivepreload.h"

#include <QIcon>
#include <QGuiApplication>
#include <QLibraryInfo>
#include <QQmlApplicationEngine>
#include <QDir>
#include <QSurfaceFormat>

TokLivePreload::TokLivePreload(QObject *parent)
    : QObject{parent}
{}

TokLivePreload::~TokLivePreload()
{

}

void TokLivePreload::aboutToPreload(QQmlApplicationEngine *engine)
{
    QSurfaceFormat format = QSurfaceFormat::defaultFormat();
    if (!format.hasAlpha() || format.alphaBufferSize() != 8) {
        format.setAlphaBufferSize(8);
    }

    QSurfaceFormat::setDefaultFormat(format);

#ifdef CC_STARTUP_PLUGIN_PATH
    engine->addImportPath(CC_STARTUP_PLUGIN_PATH);
#endif
    engine->addPluginPath(QLibraryInfo::path(QLibraryInfo::DataPath) + "/plugins");
}

QUrl TokLivePreload::preloadComponentPath() const
{
    return QUrl("qrc:///PreloadWindow.qml");
}

void TokLivePreload::preloadCreated(QQmlApplicationEngine *)
{
    qGuiApp->setApplicationName("Tok Live");
}
