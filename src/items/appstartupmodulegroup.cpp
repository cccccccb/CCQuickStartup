#include "appstartupmodulegroup.h"
#include "appstartupinstance.h"
#include "appstartupinterface.h"

#include <QQuickItem>

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

void AppStartupModuleGroup::setItemSurface(QQuickItem *item)
{
    dd->_bindingProperties.insert(BindingProperty::ItemSurface, QVariant::fromValue<QQuickItem *>(item));
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

QVariant AppStartupModuleGroup::bindingProperty(BindingProperty property) const
{
    return dd->_bindingProperties.value(property);
}

void AppStartupModuleGroup::insertBindingProperty(BindingProperty property, QVariant value)
{
    dd->_bindingProperties.insert(BindingProperty::ResolvedInterface, value);
}

void AppStartupModuleGroup::insertBindingPropertyList(BindingProperty property, QVariantList value)
{
    dd->_bindingProperties.insert(BindingProperty::ResolvedInterface, value);
}
