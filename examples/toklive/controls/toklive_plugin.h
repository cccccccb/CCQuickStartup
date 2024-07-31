#ifndef TOKLIVE_PLUGIN_H
#define TOKLIVE_PLUGIN_H

#include <QQmlExtensionPlugin>

class TokLiveQmlInstance;
class EdgeToEdgeModeHelper;
class QJSEngine;

class TokLivePlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QQmlExtensionInterface_iid)

public:
    explicit TokLivePlugin(QObject *parent = nullptr);
    ~TokLivePlugin() override;;

    void registerTypes(const char *uri) override;
    void initializeEngine(QQmlEngine *engine, const char *uri) override;
private:
    QObject *edge_to_edge_singleton_type_provider(QQmlEngine *engine, QJSEngine *scriptEngine);
    QObject *toklive_singleton_type_provider(QQmlEngine *engine, QJSEngine *scriptEngine);

private:
    QPointer<EdgeToEdgeModeHelper> edgeToEdgeHelper;
    QPointer<TokLiveQmlInstance> tokliveInstance;
};

#endif // TOKLIVE_PLUGIN_H
