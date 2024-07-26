#include "appstartupinstanceattached.h"
#include "quickappstartupitem.h"

#include <private/qmetaobjectbuilder_p.h>
#include <private/qqmldata_p.h>
#include <private/qqmlopenmetaobject_p.h>
#include <private/qqmlpropertycache_p.h>
#include <private/qquicktransition_p.h>

static QuickAppStartupItem *findStartupRootItem(QObject *obj)
{
    do {
        QuickAppStartupItem *item = qobject_cast<QuickAppStartupItem *>(obj);
        if (item)
            return item;

        obj = obj->parent();
    } while (obj);

    return nullptr;
}

class Q_DECL_HIDDEN CustomSubItemMetaObject : public QQmlOpenMetaObject
{
public:
    CustomSubItemMetaObject(AppStartupInstanceAttached *obj)
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
        : QQmlOpenMetaObject(obj, new QQmlOpenMetaObjectType(obj->metaObject(), qmlEngine(obj->parent())))
#else
        : QQmlOpenMetaObject(obj, new QQmlOpenMetaObjectType(obj->metaObject()))
#endif
    {

    }

    ~CustomSubItemMetaObject()
    {

    }

    inline AppStartupInstanceAttached *owner() const {
        return qobject_cast<AppStartupInstanceAttached *>(object());
    }

    QVariant propertyWriteValue(int id, const QVariant &v) override;
    void propertyCreated(int id, QMetaPropertyBuilder &builder) override;
    void propertyRead(int id) override;

#if QT_VERSION <= QT_VERSION_CHECK(6, 2, 4)
    int createProperty(const char *name, const char *data) override;
#endif
};

class AppStartupInstanceAttachedPrivate {
public:
    AppStartupInstanceAttachedPrivate(AppStartupInstanceAttached *qq)
        : _qq(qq)
        , _metaObject(new CustomSubItemMetaObject(qq))
    {}

    AppStartupInstanceAttached *_qq;
    QQuickTransition *_displayPopulateTransition = nullptr;
    bool _loaded = false;
    QMap<QByteArray, QObject *> _objects;
    CustomSubItemMetaObject *_metaObject;

#if QT_VERSION <= QT_VERSION_CHECK(6, 2, 4)
    QQmlPropertyCache *m_propertyCache = nullptr;
#else
    QQmlPropertyCache::ConstPtr m_propertyCache;
#endif
};

QVariant CustomSubItemMetaObject::propertyWriteValue(int id, const QVariant &v) {
    return QQmlOpenMetaObject::propertyWriteValue(id, v);
}

void CustomSubItemMetaObject::propertyCreated(int id, QMetaPropertyBuilder &builder)
{
    builder.setWritable(false);
    builder.setResettable(false);
    return QQmlOpenMetaObject::propertyCreated(id, builder);
}

void CustomSubItemMetaObject::propertyRead(int id)
{
    struct FakerItem : public QQuickItem {
        inline bool isComponentComplete() const {
            return QQuickItem::isComponentComplete();
        }
    };
    auto qmlObj = qobject_cast<QQuickItem*>(owner()->parent());
    if (qmlObj && static_cast<FakerItem*>(qmlObj)->isComponentComplete() && !value(id).canConvert<QObject *>()) {
        const auto pname = name(id);
        if (owner()->dd->_objects.value(pname))
            qWarning() << "AppStartupInstance: The" << pname << "is an invalid id on the" << qmlObj;
    }
    QQmlOpenMetaObject::propertyRead(id);
}

#if QT_VERSION <= QT_VERSION_CHECK(6, 2, 4)
int CustomSubItemMetaObject::createProperty(const char *name, const char *data)
{
    QQmlData *qmldata = QQmlData::get(owner(), false);
    auto cache = qmldata ? qmldata->propertyCache : nullptr;
    Q_ASSERT(!cache || cache == owner()->m_propertyCache);

    int ret = QQmlOpenMetaObject::createProperty(name, data);
    if (qmldata && qmldata->propertyCache != cache) {
        Q_ASSERT(!qmldata->propertyCache); // It's released in QQmlOpenMetaObject::createProperty
        // Ensure QQmlOpenMetaObject::setCached always take effect
        qmldata->propertyCache = owner()->m_propertyCache;
        qmldata->propertyCache->addref();
    }

    return ret;
}
#endif

AppStartupInstanceAttached::AppStartupInstanceAttached(QObject *parent)
    : QObject(parent)
    , dd(new AppStartupInstanceAttachedPrivate(this))
{
    dd->_metaObject->setCached(true);

    QQmlData *qmldata = QQmlData::get(this);
    Q_ASSERT(qmldata);
    dd->m_propertyCache = qmldata->propertyCache;
}

AppStartupInstanceAttached::~AppStartupInstanceAttached()
{

}

QQuickTransition *AppStartupInstanceAttached::displayPopulate() const
{
    return dd->_displayPopulateTransition;
}

void AppStartupInstanceAttached::setDisplayPopulate(QQuickTransition *transition)
{
    if (dd->_displayPopulateTransition == transition)
        return;
    dd->_displayPopulateTransition = transition;
    Q_EMIT displayPopulateChanged();
}

bool AppStartupInstanceAttached::loaded() const
{
    return dd->_loaded;
}

void AppStartupInstanceAttached::setLoaded(bool loaded)
{
    if (dd->_loaded == loaded)
        return;

    dd->_loaded = loaded;
    Q_EMIT loadedChanged();
}

AppStartupInstanceAttached *AppStartupInstanceAttached::qmlAttachedProperties(QObject *object)
{
    AppStartupInstanceAttached *attached = nullptr;

    QuickAppStartupItem *appRootItem = findStartupRootItem(object);
    if (appRootItem) {
        attached = qobject_cast<AppStartupInstanceAttached*>(qmlAttachedPropertiesObject<AppStartupInstanceAttached>(appRootItem, false));

        if (!attached)
            attached = new AppStartupInstanceAttached(appRootItem);
    }

    return attached;
}

void AppStartupInstanceAttached::setSubObject(const QByteArray &name, QObject *obj)
{
    dd->_metaObject->setValue(name, QVariant::fromValue<QObject*>(obj));
    dd->_objects.insert(name, obj);
}

#include "moc_appstartupinstanceattached.cpp"

