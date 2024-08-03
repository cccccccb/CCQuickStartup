#ifndef TOKLIVEQMLINSTANCE_H
#define TOKLIVEQMLINSTANCE_H

#include <QObject>
#include <QUrl>
#include <QQmlComponent>
#include <QColor>

class TokLiveQmlInstance : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool isAndroid READ isAndroid CONSTANT FINAL)

    QML_SINGLETON
    QML_NAMED_ELEMENT(TokLive)

public:
    explicit TokLiveQmlInstance(QObject *parent = nullptr);

    bool isAndroid() const;

    Q_INVOKABLE QUrl toTokIconUrl(const QString &name, const QColor &color, bool hovered, bool pressed);

private:
    bool m_isAndroid;
};

#endif // TOKLIVEQMLINSTANCE_H
