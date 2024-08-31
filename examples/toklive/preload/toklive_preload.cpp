#include "toklive_preload.h"

#include <QIcon>
#include <QGuiApplication>
#include <QLibraryInfo>
#include <QQmlApplicationEngine>
#include <QDir>
#include <QSurfaceFormat>

// #include <QtQml/qqmlextensionplugin.h>

// Q_IMPORT_QML_PLUGIN(TokLiveControlExtension)

static QStringList buildinPluginPaths()
{
    QStringList result = { QLibraryInfo::path(QLibraryInfo::DataPath) + "/modules" };
    const auto ccPluginPath = qgetenv("TOKLIVE_QML_MODULE_PATH");
    if (!ccPluginPath.isEmpty())
        result.append(ccPluginPath);

#ifdef TOKLIVE_QML_MODULE_PATH
    result.append(TOKLIVE_QML_MODULE_PATH);
#endif

    return result;
}

TokLivePreload::TokLivePreload(QObject *parent)
    : QObject{parent}
{}

TokLivePreload::~TokLivePreload()
{

}

void TokLivePreload::aboutToPreload(QQmlApplicationEngine *engine)
{
    qGuiApp->setApplicationName("TokLive");
    QSurfaceFormat format = QSurfaceFormat::defaultFormat();
    if (!format.hasAlpha() || format.alphaBufferSize() != 8) {
        format.setAlphaBufferSize(8);
    }

    QSurfaceFormat::setDefaultFormat(format);

    const auto &pluginList = buildinPluginPaths();
    for (const auto &path : pluginList) {
        engine->addPluginPath(path);
    }
}

QUrl TokLivePreload::preloadModulePath() const
{
    return QUrl("qrc:///qml/PreloadWindow.qml");
}
