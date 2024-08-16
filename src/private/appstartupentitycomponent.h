#ifndef APPSTARTUPENTITYCOMPONENT_H
#define APPSTARTUPENTITYCOMPONENT_H

#include "appstartupcomponent.h"

class AppStartupEntityInterface;
class AppStartupItem;

class AppStartupEntityComponent : public AppStartupComponent
{
    Q_OBJECT
    friend class AppQmlComponentIncubator;
public:
    using AppStartupComponent::AppStartupComponent;
    ~AppStartupEntityComponent();

    inline AppStartupComponentInformation::StartComponent componentType() override { return AppStartupComponentInformation::Entity; }

    QQuickItem *transitionItem() override;
    QQuickTransition *transition() override;

    AppStartupComponent *transitionLinkPrev() override;
    void transitionFinish() override;
    void beforeTransition() override;

    bool load() override;

protected:
    void itemGeometryChanged(QQuickItem *item, QQuickGeometryChange change, const QRectF &oldGeometry) override;

private Q_SLOTS:
    void _q_onEntityComponentStatusChanged(QQmlComponent::Status status);
    void _q_onComponentProgressChanged();

private:
    bool createObjects(const QQmlListReference &pros);
    void createChildComponents();
    QQmlListReference findWindowDefaultDataRef();

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
    int childrenCount = 0;
};

#endif // APPSTARTUPENTITYCOMPONENT_H
