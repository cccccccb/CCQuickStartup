#ifndef TOKLIVEPRELOAD_H
#define TOKLIVEPRELOAD_H

#include <QObject>

#include "interface/appstartuppreloadinterface.h"

class TokLivePreload : public QObject, public AppStartupPreloadInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID AppStartupPreloadInterface_iid FILE "plugin.json")
    Q_INTERFACES(AppStartupPreloadInterface)

public:
    explicit TokLivePreload(QObject *parent = nullptr);
    ~TokLivePreload();

    void aboutToPreload(QQmlApplicationEngine *engine);
    QUrl preloadModulePath() const;
};

#endif // TOKLIVEPRELOAD_H
