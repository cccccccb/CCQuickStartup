#include "ccquickstartup_plugin.h"
#include "items/appstartupinstanceattached.h"
#include "items/appstartuppreloadattached.h"
#include "items/appstartupitem.h"

#include <qqml.h>

void CCQuickStartupExtension::registerTypes(const char *uri)
{
    // @uri org.orange.quickstartup
    qmlRegisterUncreatableType<AppStartupInstanceAttached>(uri, 1, 0, "StartupItem", "StartupItem is an abstract type that is only available as an attached property.");
    qmlRegisterUncreatableType<AppStartupPreloadAttached>(uri, 1, 0, "PreloadItem", "PreloadItem is an abstract type that is only available as an attached property.");
    qmlRegisterType<AppStartupItem>(uri, 1, 0, "AppStartupItem");
}
