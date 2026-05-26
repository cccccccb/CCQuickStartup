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
    static QString nameFromQueryItem(const QString &query);
    static QColor colorFromQueryItem(const QString &query);
    static bool boolFromQueryItem(const QString &key, const QString &query);

    QPixmap requestPixmap(const QString &id, QSize *size, const QSize &requestedSize) override;
};

#endif // ICONIMAGEPROVIDER_H
