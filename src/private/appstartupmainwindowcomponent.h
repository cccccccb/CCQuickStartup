#ifndef APPSTARTUPMAINWINDOWCOMPONENT_H
#define APPSTARTUPMAINWINDOWCOMPONENT_H

#include "appstartupcomponent.h"

class CCStartupMainWindowInterface;
class AppStartupItem;

class AppStartupMainWindowComponent : public AppStartupComponent
{
    Q_OBJECT
    friend class AppQmlComponentIncubator;
public:
    using AppStartupComponent::AppStartupComponent;
    ~AppStartupMainWindowComponent();

    inline AppStartupComponentInformation::StartComponent componentType() override { return AppStartupComponentInformation::MainWindow; }

    QQuickItem *transitionItem() override;
    QQuickTransition *transition() override;

    AppStartupComponent *transitionLinkPrev() override;
    void transitionFinish() override;
    void beforeTransition() override;

    void load() override;

protected:
    void itemGeometryChanged(QQuickItem *item, QQuickGeometryChange change, const QRectF &oldGeometry) override;

private Q_SLOTS:
    void _q_onMainComponentStatusChanged(QQmlComponent::Status status);
    void _q_onComponentProgressChanged();

private:
    bool createObjects(const char *propertyName);
    void createChildComponents();
    void updateRootItemSize(QQuickItem *item);
    void destoryIncubator(QQmlIncubator *incubator);
    void finishedLoaded();
    void endOfTransition();

private:
    QScopedPointer<CCStartupMainWindowInterface> mainInstance;
    QQmlComponent *mainComponent = nullptr;
    AppStartupItem *appRootItem = nullptr;
    QList<QQmlIncubator *> incubators;

    int childrenCount = 0;
};

#endif // APPSTARTUPMAINWINDOWCOMPONENT_H
