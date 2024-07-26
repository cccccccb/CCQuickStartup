#include "tokliveqmlinstance.h"
#include "iconimageprovider.h"

#include <QQuickWindow>

TokLiveQmlInstance::TokLiveQmlInstance(QObject *parent)
    : QObject{parent}
{}

QUrl TokLiveQmlInstance::toTokIconUrl(const QString &name, const QColor &color, bool hovered, bool pressed)
{
    return IconImageProvider::toTokIconUrl(name, color, hovered, pressed);
}

bool TokLiveQmlInstance::isAndroid() const
{
#ifdef Q_OS_ANDROID
    return true;
#endif
    return false;
}
