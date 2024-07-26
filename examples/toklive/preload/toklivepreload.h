#ifndef TOKLIVEPRELOAD_H
#define TOKLIVEPRELOAD_H

#include <QObject>

#include "ccquickstartuppreloadinterface.h"

class TokLivePreload : public QObject, public CCQuickStartupPreloadInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID CCQuickStartupPreloadInterface_iid FILE "plugin.json")
    Q_INTERFACES(CCQuickStartupPreloadInterface)

public:
    explicit TokLivePreload(QObject *parent = nullptr);
    ~TokLivePreload();

    void aboutToPreload(QQmlApplicationEngine *engine);
    QUrl preloadComponentPath() const;
    void preloadCreated(QQmlApplicationEngine *engine);
};

#endif // TOKLIVEPRELOAD_H
