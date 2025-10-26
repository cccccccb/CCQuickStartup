#include "appstartupmodulegroup.h"
#include "appstartupinstance.h"
#include "appstartupinterface.h"

#include "private/appstartupinstance_p.h"

#include <QDir>
#include <QQuickItem>
#include <QHash>

class AppStartupModuleGroupPrivate
{
public:
    AppStartupModuleGroupPrivate(AppStartupModuleGroup *qq)
        : _qq(qq) {}

    AppStartupModuleGroup *_qq;
    typedef std::pair<AppStartupModuleInformation, AppStartupModuleInformation> Group;
    Group _group;
    QMap<AppStartupModuleGroup::BindingProperty, QVariant> _bindingProperties;


};

AppStartupModuleGroup::AppStartupModuleGroup(QObject *parent)
    : QObject{parent}
    , dd(new AppStartupModuleGroupPrivate(this))
{
    connect(AppStartupInstance::instance(), &AppStartupInstance::loaded, this,
            [this](const QSharedPointer<AppStartupModuleGroup> &module) {
            if (module == this) {
                Q_EMIT this->loadedChanged();
            }
    });
}

AppStartupModuleGroup::AppStartupModuleGroup(std::pair<AppStartupModuleInformation, AppStartupModuleInformation> args, QObject *parent)
    : AppStartupModuleGroup(parent)
{
    dd->_group = args;
}

AppStartupModuleGroup::~AppStartupModuleGroup()
{

}

bool AppStartupModuleGroup::isValid() const
{
    return dd->_group.first.isValid() && dd->_group.second.isValid();
}

AppStartupModuleInformation AppStartupModuleGroup::preload() const
{
    return dd->_group.first;
}

void AppStartupModuleGroup::setPreload(const AppStartupModuleInformation &preload)
{
    dd->_group.first = preload;
}

AppStartupModuleInformation AppStartupModuleGroup::entity() const
{
    return dd->_group.second;
}

void AppStartupModuleGroup::setEntity(const AppStartupModuleInformation &entity)
{
    dd->_group.second = entity;
}

bool AppStartupModuleGroup::loaded() const
{
    return AppStartupInstance::instance()->loadedModules().contains(this);
}

void AppStartupModuleGroup::setSurfaceItem(QQuickItem *item)
{
    dd->_bindingProperties.insert(BindingProperty::SurfaceItem, QVariant::fromValue<QQuickItem *>(item));
}

QList<AppStartupInterface *> AppStartupModuleGroup::resolveInterface(int type)
{
    QVariantList valueList = dd->_bindingProperties.value(BindingProperty::ResolvedInterface).toList();

    QList<AppStartupInterface *> result;
    for (const QVariant &value : std::as_const(valueList)) {
        if (!value.canConvert<AppStartupInterface *>())
            continue;

        AppStartupInterface *interface = value.value<AppStartupInterface *>();
        if (interface->type() != type)
            continue;

        result << interface;
    }

    return result;
}

QList<QSharedPointer<AppStartupModuleGroup>> AppStartupModuleGroup::loadFromPath(const QString &path)
{
    QUrl pathUrl(path);
    if (!pathUrl.isLocalFile())
        return {};

    QDir pathDir(path);
    if (!pathDir.exists())
        return {};

    QList<AppStartupModuleInformation> informations;
    QList<QSharedPointer<AppStartupModuleGroup>> result;

    for (const auto &entry : pathDir.entryInfoList(QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot)) {
        const auto path = entry.absoluteFilePath();
        AppStartupModuleInformation resolved;
        if (!AppStartupInstancePrivate::resolveInformation(path, &resolved))
            continue;

        for (auto it = informations.begin(); it != informations.end(); ++it) {
            AppStartupModuleInformation info = *it;

            if (info.startModule() == resolved.startModule()
                    || info.appId() != resolved.appId()
                    || info.descriptor() != resolved.descriptor()
                    || info.version() != resolved.version())
                continue;

            // is same group module
            AppStartupModuleInformation preload = resolved, entity = info;
            if (info.startModule() == AppStartupModuleInformation::Preload) {
                preload = info;
                entity = resolved;
            }

            auto module = QSharedPointer<AppStartupModuleGroup>::create(std::make_pair(preload, entity));
            if (!module->isValid())
                continue;

            result << module;
            informations.erase(it);
            break;
        }
    }

    return result;
}

QVariant AppStartupModuleGroup::bindingProperty(BindingProperty property) const
{
    return dd->_bindingProperties.value(property);
}

void AppStartupModuleGroup::insertBindingProperty(BindingProperty property, QVariant value)
{
    dd->_bindingProperties.insert(property, value);
}

void AppStartupModuleGroup::insertBindingPropertyList(BindingProperty property, QVariantList value)
{
    dd->_bindingProperties.insert(property, value);
}
