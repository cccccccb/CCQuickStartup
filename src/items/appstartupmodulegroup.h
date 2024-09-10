#ifndef APPSTARTUPMODULEGROUP_H
#define APPSTARTUPMODULEGROUP_H

#include "ccquickstartup_global.h"
#include "appstartupmoduleinformation.h"

#include <QObject>

class QQuickItem;
class AppStartupInterface;
class AppStartupModuleGroupPrivate;

class CC_QUICKSTARTUP_EXPORT AppStartupModuleGroup : public QObject
{
    Q_OBJECT
    Q_PROPERTY(AppStartupModuleInformation preload READ preload CONSTANT FINAL)
    Q_PROPERTY(AppStartupModuleInformation entity READ entity CONSTANT FINAL)
    Q_PROPERTY(bool loaded READ loaded NOTIFY loadedChanged FINAL)

public:
    explicit AppStartupModuleGroup(QObject *parent = nullptr);
    AppStartupModuleGroup(std::pair<AppStartupModuleInformation, AppStartupModuleInformation> args, QObject *parent = nullptr);
    ~AppStartupModuleGroup();

    bool isValid() const;

    AppStartupModuleInformation preload() const;
    void setPreload(const AppStartupModuleInformation &preload);

    AppStartupModuleInformation entity() const;
    void setEntity(const AppStartupModuleInformation &entity);

    bool loaded() const;

    void setSurfaceItem(QQuickItem *item);
    QList<AppStartupInterface *> resolveInterface(int type);

    enum BindingProperty {
        SurfaceItem,
        ResolvedInterface
    };

Q_SIGNALS:
    void loadedChanged();

protected:
    QVariant bindingProperty(BindingProperty property) const;
    void insertBindingProperty(BindingProperty property, QVariant value);
    void insertBindingPropertyList(BindingProperty property, QVariantList value);

private:
    friend class AppStartupModuleObject;
    friend class AppStartupModuleGroupPrivate;
    QScopedPointer<AppStartupModuleGroupPrivate> dd;
};

#endif // APPSTARTUPMODULEGROUP_H
