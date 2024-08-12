#ifndef TOKLIVE_PLUGIN_H
#define TOKLIVE_PLUGIN_H

#include <QQmlExtensionPlugin>

class TokLiveQmlInstance;
class EdgeToEdgeModeHelper;
class QJSEngine;

class TokLiveControlExtension : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QQmlExtensionInterface_iid)

public:
    explicit TokLiveControlExtension(QObject *parent = nullptr);
    ~TokLiveControlExtension() override;;

    void registerTypes(const char *uri) override;
    void initializeEngine(QQmlEngine *engine, const char *uri) override;

    QObject *edge_to_edge_singleton_type_provider(QQmlEngine *engine, QJSEngine *scriptEngine);
    QObject *toklive_singleton_type_provider(QQmlEngine *engine, QJSEngine *scriptEngine);

private:
    QPointer<EdgeToEdgeModeHelper> edgeToEdgeHelper;
    QPointer<TokLiveQmlInstance> tokliveInstance;
};

#endif // TOKLIVE_PLUGIN_H
