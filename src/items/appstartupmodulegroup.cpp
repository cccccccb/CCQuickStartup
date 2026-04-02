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

    AppStartupModuleGroup::Status _status = AppStartupModuleGroup::Idle;
    QString _errorString;
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

    connect(AppStartupInstance::instance(), &AppStartupInstance::errorOccured, this,
            [this](const QSharedPointer<AppStartupModuleGroup> &module, const QString &errorString) {
            if (module == this) {
                Q_EMIT this->errorOccured(errorString);
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

bool AppStartupModuleGroup::operator==(const AppStartupModuleGroup &other) const
{
    return isValid() ? (other.preload() == this->preload()
                        && other.entity() == this->entity())
                     : true;
}

bool AppStartupModuleGroup::operator!=(const AppStartupModuleGroup &other) const
{
    return !(this->operator==(other));
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

AppStartupModuleGroup::Status AppStartupModuleGroup::status() const
{
    return dd->_status;
}

void AppStartupModuleGroup::setStatus(Status status)
{
    if (dd->_status == status)
        return;

    dd->_status = status;
    Q_EMIT statusChanged();

    if (status == Ready) {
        Q_EMIT loadedChanged();
    }
}

QString AppStartupModuleGroup::errorString() const
{
    return dd->_errorString;
}

void AppStartupModuleGroup::setError(const QString &errorString)
{
    dd->_errorString = errorString;
    setStatus(Error);
    Q_EMIT errorStringChanged();
    Q_EMIT errorOccured(errorString);
}

bool AppStartupModuleGroup::loaded() const
{
    return dd->_status == Ready;
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

    const auto &entryList = pathDir.entryInfoList(QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot);
    for (const auto &entry : entryList) {
        const auto path = entry.absoluteFilePath();
        AppStartupModuleInformation resolved;
        if (!AppStartupInstancePrivate::resolveInformation(path, &resolved))
            continue;

        informations << resolved;
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
