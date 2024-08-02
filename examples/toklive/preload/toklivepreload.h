#ifndef TOKLIVEPRELOAD_H
#define TOKLIVEPRELOAD_H

#include <QObject>

#include "ccstartuppreloadinterface.h"

class TokLivePreload : public QObject, public CCStartupPreloadInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID CCStartupPreloadInterface_iid FILE "plugin.json")
    Q_INTERFACES(CCStartupPreloadInterface)

public:
    explicit TokLivePreload(QObject *parent = nullptr);
    ~TokLivePreload();

    void aboutToPreload(QQmlApplicationEngine *engine);
    QUrl preloadComponentPath() const;
};

#endif // TOKLIVEPRELOAD_H
