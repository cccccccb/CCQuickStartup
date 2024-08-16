#ifndef DEFINES_P_H
#define DEFINES_P_H

#include <private/qquickitem_p.h>

#define APPLICATIONWINDOW_CONTENTDATA "contentData"
#define WINDOW_OR_ITEM_CONTENTDATA "data"

static inline bool heightValid(QQuickItemPrivate *item)
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    return item->heightValid;
#else
    return item->heightValid();
#endif
}

static inline bool widthValid(QQuickItemPrivate *item)
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    return item->widthValid;
#else
    return item->widthValid();
#endif
}

#endif // DEFINES_P_H
