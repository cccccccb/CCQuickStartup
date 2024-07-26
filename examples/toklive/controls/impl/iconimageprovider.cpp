#include "iconimageprovider.h"
#include <QIcon>
#include <QPainter>
#include <QPixmap>
#include <QPixmapCache>
#include <QUrlQuery>

IconImageProvider::IconImageProvider()
    : QQuickImageProvider(QQuickImageProvider::Pixmap)
{ }

QUrl IconImageProvider::toTokIconUrl(const QString &name, const QColor &color, bool hovered, bool pressed)
{
    QUrl url;
    url.setScheme("image");
    url.setHost("toklive.icons");

    QUrlQuery args;
    args.addQueryItem("name", name);
    args.addQueryItem("color", color.name(QColor::HexArgb));
    args.addQueryItem("hovered", QString::number(hovered ? 1 : 0));
    args.addQueryItem("pressed", QString::number(pressed ? 1 : 0));

    url.setQuery(args);

    return url;
}

QString IconImageProvider::getNameFromQueryItem(const QString &query)
{
    QUrlQuery args(query);

    return args.queryItemValue("name");
}

QColor IconImageProvider::getColorFromQueryItem(const QString &query)
{
    QUrlQuery args(query);

    return QColor(args.queryItemValue("color"));
}

bool IconImageProvider::getBoolFromQueryItem(const QString &key, const QString &query)
{
    QUrlQuery args(query);

    return args.queryItemValue(key).toInt();
}

QPixmap IconImageProvider::requestPixmap(const QString &id, QSize *size, const QSize &requestedSize)
{
    const QString &iconName = getNameFromQueryItem(id);
    QColor iconColor = getColorFromQueryItem(id);
    bool colorIsValid = iconColor.isValid() && iconColor.alpha() > 0;

    if (colorIsValid) {
        if (getBoolFromQueryItem("pressed", id)) {
            iconColor = iconColor.darker(120);
        } else if (getBoolFromQueryItem("hovered", id)) {
            iconColor = iconColor.lighter(120);
        }
    }

    QSize iconSize = requestedSize.isValid() ? requestedSize : QSize(32, 32);
    const QString &sizeString = QStringLiteral("%1x%2").arg(iconSize.width()).arg(iconSize.height());
    QString key = QStringLiteral("_tok_icon_cached:%1:%2").arg(iconName, sizeString);
    if (colorIsValid) {
        key += QLatin1String(":") + iconColor.name(QColor::HexArgb);
    }

    QPixmap pixmap;
    if (!QPixmapCache::find(key, &pixmap)) {
        QIcon icon = QIcon::fromTheme(iconName);
        if (icon.isNull()) {
            return pixmap;
        }

        pixmap = icon.pixmap(iconSize);
        if (pixmap.isNull())
            return pixmap;

        if (colorIsValid) {
            QPainter painter(&pixmap);
            painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
            painter.fillRect(pixmap.rect(), iconColor);
        }

        if (pixmap.size() !=  iconSize) {
            pixmap = pixmap.scaled(iconSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        }

        QPixmapCache::insert(key, pixmap);
    }

    if (size) {
        *size = pixmap.size();
    }

    return pixmap;
}
