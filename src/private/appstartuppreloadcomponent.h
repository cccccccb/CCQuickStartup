#ifndef APPSTARTUPPRELOADCOMPONENT_H
#define APPSTARTUPPRELOADCOMPONENT_H

#include "appstartupcomponent.h"

#include "items/apppreloaditem.h"

class AppStartupPreloadInterface;

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

    void createWindow();
    void findWindowContentItem();
    void createOverlay();
    void clearOverlay();
    void doOverlayAutoExitChanged(AppPreloadItem *attached);
    AppPreloadItem *appPreloadItem() const;

private:
    AppStartupPreloadInterface *preloadInstance = nullptr;
    QQuickItem *loadingOverlay = nullptr;
    bool overlayUsingParentSize = false;
    QVariantHash initialPropertiesHash;
};

#endif // APPSTARTUPPRELOADCOMPONENT_H
