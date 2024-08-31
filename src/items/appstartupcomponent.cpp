#include "appstartupcomponent.h"
#include "appstartupinitialproperties.h"

#include <QQmlComponent>

class AppStartupComponentPrivate
{
public:
    AppStartupComponentPrivate(AppStartupComponent *qq)
        : _qq(qq)
    {}

    AppStartupComponent *_qq = nullptr;
    AppStartupInitialProperties *_initialProperties = nullptr;
    QVector<AppStartupComponent *> _depends;
    QQmlComponent *_target = nullptr;
};

AppStartupComponent::AppStartupComponent(QObject *parent)
    : QObject{parent}
    , dd(new AppStartupComponentPrivate(this))
{

}

AppStartupComponent::~AppStartupComponent()
{

}

AppStartupInitialProperties *AppStartupComponent::initialProperties() const
{
    return dd->_initialProperties;
}

void AppStartupComponent::setInitialProperties(AppStartupInitialProperties *initialProperties)
{
    if (dd->_initialProperties == initialProperties)
        return;

    dd->_initialProperties = initialProperties;
    Q_EMIT initialPropertiesChanged();
}

QQmlComponent *AppStartupComponent::target() const
{
    return dd->_target;
}

void AppStartupComponent::setTarget(QQmlComponent *target)
{
    if (dd->_target == target)
        return;
    dd->_target = target;
    Q_EMIT targetChanged();
}

QQmlListProperty<AppStartupComponent> AppStartupComponent::depends()
{
    return {this, &dd->_depends};
}

#include "moc_appstartupcomponent.cpp"
