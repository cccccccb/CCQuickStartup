#include "ccquickstartup_plugin.h"
#include "items/appstartupitemattached.h"
#include "items/appstartuppreloaditem.h"
#include "items/appstartupitem.h"
#include "items/appstartuptransitiongroup.h"
#include "items/appstartupinitialproperties.h"

#include <qqml.h>

void CCQuickStartupExtension::registerTypes(const char *uri)
{
    // @uri org.orange.quickstartup
    qmlRegisterUncreatableType<AppStartupItemAttached>(uri, 1, 0, "AppStartupItem", "AppStartupItem is an abstract type that is only available as an attached property.");
    qmlRegisterType<AppPreloadItem>(uri, 1, 0, "AppPreloadItem");
    qmlRegisterType<AppStartupItem>(uri, 1, 0, "AppStartupItem");
    qmlRegisterType<AppStartupTransitionGroup>(uri, 1, 0, "TransitionGroup");
    qmlRegisterType<AppStartupInitialProperties>(uri, 1, 0, "InitialProperties");
    qmlRegisterType(QUrl("qrc:/appstartup/qml/tansition/FlickTransition.qml"), uri, 1, 0, "FlickTransition");
}
