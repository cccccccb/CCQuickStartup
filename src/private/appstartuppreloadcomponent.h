#ifndef APPSTARTUPPRELOADCOMPONENT_H
#define APPSTARTUPPRELOADCOMPONENT_H

#include "appstartupcomponent.h"

class CCStartupPreloadInterface;
class AppStartupPreloadAttached;

class AppStartupPreloadComponent : public AppStartupComponent
{
    Q_OBJECT
public:
    using AppStartupComponent::AppStartupComponent;
    ~AppStartupPreloadComponent();

    inline AppStartupComponentInformation::StartComponent componentType() override
    {
        return AppStartupComponentInformation::Preload;
    };

    inline QQuickItem *transitionItem() override { return loadingOverlay; }
    virtual QQuickTransition *transition() override { return overlayExitTransition; }

    AppStartupComponent *transitionLinkNext() override;
    void transitionFinish() override;

    virtual void load() override;

protected:
    void itemGeometryChanged(QQuickItem *item, QQuickGeometryChange change, const QRectF &oldGeometry) override;

private Q_SLOTS:
    void _q_onPreloadCreated(QObject *obj, const QUrl &);
    void _q_onWindowVisibleChanged(bool visible);
    void _q_onOverlayExitWhenChanged(bool changed);

    void findWindowContentItem();
    void createOverlay();
    void clearOverlay();
    void doOverlayAutoExitChanged(AppStartupPreloadAttached *attached);

private:
    CCStartupPreloadInterface *preloadInstance = nullptr;
    QQuickItem *loadingOverlay = nullptr;
    QMetaObject::Connection visibleConnection;
    QMetaObject::Connection autoExitConnection;
    QQuickTransition *overlayExitTransition = nullptr;
    bool overlayUsingParentSize = false;
};

#endif // APPSTARTUPPRELOADCOMPONENT_H
