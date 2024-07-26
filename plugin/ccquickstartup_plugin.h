#ifndef CCQUICKSTARTUP_PLUGIN_H
#define CCQUICKSTARTUP_PLUGIN_H

#include <QQmlExtensionPlugin>

class CCQuickStartupExtension : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QQmlExtensionInterface_iid)

public:
    void registerTypes(const char *uri) override;
};

#endif // CCQUICKSTARTUP_PLUGIN_H
