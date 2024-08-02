#include "toklive_plugin.h"
#include "impl/edgetoedgemodehelper.h"
#include "impl/tokliveqmlinstance.h"
#include "impl/iconimageprovider.h"
#include "impl/framelessattached.h"

#include <qqml.h>
#include <QDebug>

TokLivePlugin::TokLivePlugin(QObject *parent)
    : QQmlExtensionPlugin(parent)
{

}

TokLivePlugin::~TokLivePlugin()
{
    if (edgeToEdgeHelper) {
        delete edgeToEdgeHelper;
        edgeToEdgeHelper = nullptr;
    }

    if (tokliveInstance) {
        delete tokliveInstance;
        tokliveInstance = nullptr;
    }
}

QObject *TokLivePlugin::edge_to_edge_singleton_type_provider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)
    if (!edgeToEdgeHelper)
        edgeToEdgeHelper = new EdgeToEdgeModeHelper;

    return edgeToEdgeHelper;
}

QObject *TokLivePlugin::toklive_singleton_type_provider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)
    if (!tokliveInstance)
        tokliveInstance = new TokLiveQmlInstance;

    return tokliveInstance;
}

void TokLivePlugin::registerTypes(const char *uri)
{
    QByteArray implUri = QByteArray(uri).append(".impl");
    qmlRegisterModule(implUri.constData(), 1, 0);

    // @uri org.orange.toklive.impl
    qmlRegisterSingletonType<EdgeToEdgeModeHelper *>(implUri, 1, 0, "EdgeToEdge", std::bind(&TokLivePlugin::edge_to_edge_singleton_type_provider, this,
                                                                                        std::placeholders::_1, std::placeholders::_2));
    qmlRegisterSingletonType<TokLiveQmlInstance *>(implUri, 1, 0, "TokLive", std::bind(&TokLivePlugin::toklive_singleton_type_provider, this,
                                                                                   std::placeholders::_1, std::placeholders::_2));

    // @uri org.orange.toklive
    qmlRegisterSingletonType<EdgeToEdgeModeHelper *>(uri, 1, 0, "EdgeToEdge", std::bind(&TokLivePlugin::edge_to_edge_singleton_type_provider, this,
                                                                                        std::placeholders::_1, std::placeholders::_2));
    qmlRegisterSingletonType<TokLiveQmlInstance *>(uri, 1, 0, "TokLive", std::bind(&TokLivePlugin::toklive_singleton_type_provider, this,
                                                                                   std::placeholders::_1, std::placeholders::_2));
    qmlRegisterSingletonType(QUrl("qrc:///Style/Style.qml"), uri, 1, 0, "Style");
    qmlRegisterUncreatableType<FramelessAttached>(uri, 1, 0, "Frameless", "Frameless is an abstract type that is only available as an attached property.");

    qmlRegisterType(QUrl("qrc:/Controls/CheckedAnimateButton.qml"), uri, 1, 0, "CheckedAnimateButton");
    qmlRegisterType(QUrl("qrc:/Controls/CustomButton.qml"), uri, 1, 0, "CustomButton");
    qmlRegisterType(QUrl("qrc:/Controls/LineEdit.qml"), uri, 1, 0, "LineEdit");
    qmlRegisterType(QUrl("qrc:/Controls/SearchEdit.qml"), uri, 1, 0, "SearchEdit");
    qmlRegisterType(QUrl("qrc:/Controls/TokIcon.qml"), uri, 1, 0, "TokIcon");
}

void TokLivePlugin::initializeEngine(QQmlEngine *engine, const char *uri)
{
    engine->addImageProvider("toklive.icons", new IconImageProvider());
    engine->imageProvider("toklive.icons");

    static_cast<EdgeToEdgeModeHelper *>(edge_to_edge_singleton_type_provider(nullptr, nullptr))->enable();
    QQmlExtensionPlugin::initializeEngine(engine, uri);
}
