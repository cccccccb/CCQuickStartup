#include "toklive_plugin.h"
#include "impl/framelessattached.h"
#include "impl/edgetoedgemodehelper.h"
#include "impl/iconimageprovider.h"
#include "impl/tokliveqmlinstance.h"

#include <qqml.h>
#include <QDebug>

QT_DECLARE_EXTERN_SYMBOL_VOID(qml_register_types_org_orange_toklive)

TokLiveControlExtension::TokLiveControlExtension(QObject *parent)
    : QQmlExtensionPlugin(parent)
{
    qmlRegisterTypesAndRevisions<EdgeToEdgeModeHelper>("org.orange.toklive", 1);
    qmlRegisterTypesAndRevisions<FramelessAttached>("org.orange.toklive", 1);
    qmlRegisterTypesAndRevisions<TokLiveQmlInstance>("org.orange.toklive", 1);
    qmlRegisterModule("org.orange.toklive", 1, 0);
}

TokLiveControlExtension::~TokLiveControlExtension()
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

QObject *TokLiveControlExtension::edge_to_edge_singleton_type_provider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)
    if (!edgeToEdgeHelper)
        edgeToEdgeHelper = new EdgeToEdgeModeHelper;

    return edgeToEdgeHelper;
}

QObject *TokLiveControlExtension::toklive_singleton_type_provider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)
    if (!tokliveInstance)
        tokliveInstance = new TokLiveQmlInstance;

    return tokliveInstance;
}

void TokLiveControlExtension::registerTypes(const char *uri)
{
    QByteArray implUri = QByteArray(uri).append(".impl");
    qmlRegisterModule(implUri.constData(), 1, 0);

    // @uri org.orange.toklive.impl
    qmlRegisterSingletonType<EdgeToEdgeModeHelper *>(implUri, 1, 0, "EdgeToEdge", std::bind(&TokLiveControlExtension::edge_to_edge_singleton_type_provider, this,
                                                                                            std::placeholders::_1, std::placeholders::_2));
    qmlRegisterSingletonType<TokLiveQmlInstance *>(implUri, 1, 0, "TokLive", std::bind(&TokLiveControlExtension::toklive_singleton_type_provider, this,
                                                                                       std::placeholders::_1, std::placeholders::_2));

    // @uri org.orange.toklive
    qmlRegisterSingletonType<EdgeToEdgeModeHelper *>(uri, 1, 0, "EdgeToEdge", std::bind(&TokLiveControlExtension::edge_to_edge_singleton_type_provider, this,
                                                                                        std::placeholders::_1, std::placeholders::_2));
    qmlRegisterSingletonType<TokLiveQmlInstance *>(uri, 1, 0, "TokLive", std::bind(&TokLiveControlExtension::toklive_singleton_type_provider, this,
                                                                                   std::placeholders::_1, std::placeholders::_2));
    qmlRegisterUncreatableType<FramelessAttached>(uri, 1, 0, "Frameless", "Frameless is an abstract type that is only available as an attached property.");

    QString qmlUriPrefix(QLatin1String("qrc:/") + QString(uri).replace(".", "/") + QLatin1String("/modules"));


    qmlRegisterSingletonType(QUrl(qmlUriPrefix + "/Style/Style.qml"), uri, 1, 0, "Style");
    qmlRegisterType(QUrl(qmlUriPrefix + "/Controls/CheckedAnimateButton.qml"), uri, 1, 0, "CheckedAnimateButton");
    qmlRegisterType(QUrl(qmlUriPrefix + "/Controls/CustomButton.qml"), uri, 1, 0, "CustomButton");
    qmlRegisterType(QUrl(qmlUriPrefix + "/Controls/LineEdit.qml"), uri, 1, 0, "LineEdit");
    qmlRegisterType(QUrl(qmlUriPrefix + "/Controls/SearchEdit.qml"), uri, 1, 0, "SearchEdit");
    qmlRegisterType(QUrl(qmlUriPrefix + "/Controls/TokIcon.qml"), uri, 1, 0, "TokIcon");
}


void TokLiveControlExtension::initializeEngine(QQmlEngine *engine, const char *uri)
{
    engine->addImageProvider("toklive.icons", new IconImageProvider());
    engine->imageProvider("toklive.icons");

    if (EdgeToEdgeModeHelper *edgeToEdge = engine->singletonInstance<EdgeToEdgeModeHelper *>(qmlTypeId(uri, 1, 0, "EdgeToEdge"))) {
        edgeToEdge->enable();
    }
}

#include "toklive_plugin.moc"
