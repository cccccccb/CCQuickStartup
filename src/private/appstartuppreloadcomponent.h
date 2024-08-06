#ifndef APPSTARTUPPRELOADCOMPONENT_H
#define APPSTARTUPPRELOADCOMPONENT_H

#include "appstartupcomponent.h"

class AppStartupPreloadInterface;
class AppStartupPreloadAttached;

class AppStartupPreloadComponent : public AppStartupComponent
{
    Q_OBJECT
public:
    using AppStartupComponent::AppStartupComponent;
    ~AppStartupPreloadComponent();

    inline AppStartupComponentInformation::StartComponent componentType() override { return AppStartupComponentInformation::Preload; }

    inline QQuickItem *transitionItem() override { return loadingOverlay; }
    virtual QQuickTransition *transition() override;

    AppStartupComponent *transitionLinkNext() override;
    void transitionFinish() override;
    void beforeTransition() override;

    virtual bool load() override;

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
    AppStartupPreloadInterface *preloadInstance = nullptr;
    QQuickItem *loadingOverlay = nullptr;
    QMetaObject::Connection visibleConnection;
    QMetaObject::Connection autoExitConnection;
    bool overlayUsingParentSize = false;
    QVariantHash initialPropertiesHash;
};

#endif // APPSTARTUPPRELOADCOMPONENT_H
