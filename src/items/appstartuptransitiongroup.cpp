#include "appstartuptransitiongroup.h"
#include "appstartupinitialproperties.h"

#include <private/qquicktransition_p.h>

class AppStartupTransitionGroupPrivate {
public:
    AppStartupTransitionGroupPrivate(AppStartupTransitionGroup *qq)
        : _qq(qq)
    {}

    AppStartupTransitionGroup *_qq;
    QQuickTransition *_d_leave = nullptr;
    QQuickTransition *_d_enter = nullptr;
    AppStartupInitialProperties *_leaveInitialProperties = nullptr;
    AppStartupInitialProperties *_enterInitialProperties = nullptr;
};

AppStartupTransitionGroup::AppStartupTransitionGroup(QObject *parent)
    : QObject{parent}
    , dd(new AppStartupTransitionGroupPrivate(this))
{}

AppStartupTransitionGroup::~AppStartupTransitionGroup()
{

}

QQuickTransition *AppStartupTransitionGroup::leave() const
{
    return dd->_d_leave;
}

void AppStartupTransitionGroup::setLeave(QQuickTransition *leave)
{
    if (dd->_d_leave == leave)
        return;

    dd->_d_leave = leave;
    Q_EMIT leaveChanged();
}

QQuickTransition *AppStartupTransitionGroup::enter() const
{
    return dd->_d_enter;
}

void AppStartupTransitionGroup::setEnter(QQuickTransition *enter)
{
    if (dd->_d_enter == enter)
        return;

    dd->_d_enter = enter;
    Q_EMIT enterChanged();
}

AppStartupInitialProperties *AppStartupTransitionGroup::leaveInitialProperties() const
{
    return dd->_leaveInitialProperties;
}

void AppStartupTransitionGroup::setLeaveInitialProperties(AppStartupInitialProperties *leaveInitialProperties)
{
    if (dd->_leaveInitialProperties == leaveInitialProperties)
        return;

    dd->_leaveInitialProperties = leaveInitialProperties;
    Q_EMIT leaveInitialPropertiesChanged();
}

AppStartupInitialProperties *AppStartupTransitionGroup::enterInitialProperties() const
{
    return dd->_enterInitialProperties;
}

void AppStartupTransitionGroup::setEnterInitialProperties(AppStartupInitialProperties *enterInitialProperties)
{
    if (dd->_enterInitialProperties == enterInitialProperties)
        return;

    dd->_enterInitialProperties = enterInitialProperties;
    Q_EMIT leaveInitialPropertiesChanged();
}

#include "moc_appstartuptransitiongroup.cpp"
