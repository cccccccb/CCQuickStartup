#ifndef TOKLIVEENTITY_H
#define TOKLIVEENTITY_H

#include <QObject>

#include "interface/appstartupentityinterface.h"

class TokLiveQmlInstance;

class TokLiveEntity : public QObject, public AppStartupEntityInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID AppStartupEntityInterface_iid FILE "plugin.json")
    Q_INTERFACES(AppStartupEntityInterface)

public:
    explicit TokLiveEntity(QObject *parent = nullptr);
    ~TokLiveEntity();

    QUrl entityComponentPath() const;
    void initialize(QQmlApplicationEngine *engine);
};

#endif // TOKLIVEENTITY_H
