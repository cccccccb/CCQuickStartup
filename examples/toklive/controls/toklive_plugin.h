#ifndef TOKLIVE_PLUGIN_H
#define TOKLIVE_PLUGIN_H

#include <QQmlExtensionPlugin>

class TokLivePlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QQmlExtensionInterface_iid)

public:
    explicit TokLivePlugin(QObject *parent = nullptr);
    ~TokLivePlugin() override;;

    void registerTypes(const char *uri) override;
    void initializeEngine(QQmlEngine *engine, const char *uri) override;
};

#endif // TOKLIVE_PLUGIN_H
