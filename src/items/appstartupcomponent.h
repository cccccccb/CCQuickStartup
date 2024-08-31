#ifndef APPSTARTUPCOMPONENT_H
#define APPSTARTUPCOMPONENT_H

#include "ccquickstartup_global.h"

#include <QObject>
#include <QQmlEngine>
#include <QQmlListProperty>

class QQmlComponent;

class AppStartupComponentPrivate;
class AppStartupInitialProperties;

class CC_QUICKSTARTUP_EXPORT AppStartupComponent : public QObject
{
    Q_OBJECT
    Q_PROPERTY(AppStartupInitialProperties *initialProperties READ initialProperties WRITE setInitialProperties NOTIFY initialPropertiesChanged FINAL)
    Q_PROPERTY(QQmlListProperty<AppStartupComponent> depends READ depends CONSTANT FINAL)
    Q_PROPERTY(QQmlComponent *target READ target WRITE setTarget NOTIFY targetChanged FINAL)

    Q_CLASSINFO("DefaultProperty", "target")
    QML_NAMED_ELEMENT(AppStartupComponent)

public:
    explicit AppStartupComponent(QObject *parent = nullptr);
    ~AppStartupComponent();

    AppStartupInitialProperties * initialProperties() const;
    void setInitialProperties(AppStartupInitialProperties *newInitialProperties);

    QQmlComponent *target() const;
    void setTarget(QQmlComponent *newTarget);

    QQmlListProperty<AppStartupComponent> depends();
    //! @todo shared context ?

Q_SIGNALS:
    void initialPropertiesChanged();
    void targetChanged();

private:
    QScopedPointer<AppStartupComponentPrivate> dd;
};

#endif // APPSTARTUPCOMPONENT_H
