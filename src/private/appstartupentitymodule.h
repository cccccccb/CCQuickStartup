#ifndef APPSTARTUPENTITYMODULE_H
#define APPSTARTUPENTITYMODULE_H

#include "appstartupmodule.h"

class AppStartupComponent;
class AppStartupEntityInterface;
class AppStartupItem;

class AppStartupEntityModule : public AppStartupModule
{
    Q_OBJECT
    friend class AppQmlComponentIncubator;
public:
    using AppStartupModule::AppStartupModule;
    ~AppStartupEntityModule();

    inline AppStartupModuleInformation::StartModule moduleType() override { return AppStartupModuleInformation::Entity; }

    QQuickItem *transitionItem() override;
    QQuickTransition *transition() override;

    AppStartupModule *transitionLinkPrev() override;
    void transitionFinish() override;
    void beforeTransition() override;

    bool load() override;

protected:
    void itemGeometryChanged(QQuickItem *item, QQuickGeometryChange change, const QRectF &oldGeometry) override;

private Q_SLOTS:
    void _q_onEntityModuleStatusChanged(QQmlComponent::Status status);
    void _q_onComponentProgressChanged();

private:
    bool createObjects(const QQmlListReference &pros);
    void createChildComponents();
    void createComponnet(AppStartupComponent *component);
    QQmlListReference findWindowDefaultDataRef();

    struct ComponentDependency
    {
        QSet<AppStartupComponent *> dependsOn;
        QSet<AppStartupComponent *> beingDepends;
    };

    void updateRootItemSize(QQuickItem *item);
    void destoryIncubator(QQmlIncubator *incubator);
    void finishedLoaded();
    void endOfTransition();
    AppStartupItem *appRootItem() const;

private:
    AppStartupEntityInterface *entityInstance;

    QQmlComponent *entityComponent = nullptr;
    QList<QQmlIncubator *> incubators;
    QVariantHash initialPropertiesHash;
    QHash<AppStartupComponent *, ComponentDependency *> componentDependencyHash;
    int childrenCount = 0;
};

#endif // APPSTARTUPENTITYMODULE_H
