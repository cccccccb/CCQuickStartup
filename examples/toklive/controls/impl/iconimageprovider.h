#ifndef ICONIMAGEPROVIDER_H
#define ICONIMAGEPROVIDER_H

#include <QQuickImageProvider>
#include <QIcon>
#include <QUrl>
#include <QColor>

class IconImageProvider : public QQuickImageProvider
{
public:
    IconImageProvider();

    static QUrl toTokIconUrl(const QString &name, const QColor &color, bool hovered, bool pressed);
    static QString getNameFromQueryItem(const QString &query);
    static QColor getColorFromQueryItem(const QString &query);
    static bool getBoolFromQueryItem(const QString &key, const QString &query);

    QPixmap requestPixmap(const QString &id, QSize *size, const QSize &requestedSize) override;
};

#endif // ICONIMAGEPROVIDER_H
