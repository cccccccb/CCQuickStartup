#ifndef APPSTARTUPINITIALPROPERTIES_H
#define APPSTARTUPINITIALPROPERTIES_H

#include "ccquickstartup_global.h"

#include <QtQml>

class QQuickItem;

class CC_QUICKSTARTUP_EXPORT AppStartupInitialProperties : public QObject
{
    Q_OBJECT
    QML_NAMED_ELEMENT(InitialProperties)
    QML_ANONYMOUS

public:
    explicit AppStartupInitialProperties(QObject *parent = nullptr);
    ~AppStartupInitialProperties() = default;
};

#endif // APPSTARTUPINITIALPROPERTIES_H
