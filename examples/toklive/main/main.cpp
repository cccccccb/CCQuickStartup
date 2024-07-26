#include <QGuiApplication>
#include <QIcon>
#include <QFontDatabase>
#include <QStandardPaths>
#include <QDir>
#include <QPluginLoader>
#include <QLibraryInfo>

#include "appstartupinstance.h"

int main(int argc, char *argv[])
{
    AppStartupInstance startupInstance("org.orange.toklive");

#ifdef CC_STARTUP_PLUGIN_PATH
    startupInstance.addPluginPath(CC_STARTUP_PLUGIN_PATH);
#endif

    startupInstance.addPluginPath(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + QDir::separator() + QLatin1String("startup"));
    startupInstance.addPluginPath(QLibraryInfo::path(QLibraryInfo::DataPath));
    return startupInstance.exec(argc, argv);
}
