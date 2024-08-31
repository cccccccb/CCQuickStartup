#ifndef APPSTARTUPPRELOADMODULE_H
#define APPSTARTUPPRELOADMODULE_H

#include "appstartupmodule.h"

#include "items/appstartuppreloaditem.h"

class AppStartupPreloadInterface;

class AppStartupPreloadModule : public AppStartupModule
{
    Q_OBJECT
public:
    using AppStartupModule::AppStartupModule;
    ~AppStartupPreloadModule();

    inline AppStartupModuleInformation::StartModule moduleType() override { return AppStartupModuleInformation::Preload; }

    inline QQuickItem *transitionItem() override { return loadingOverlay; }
    virtual QQuickTransition *transition() override;

    AppStartupModule *transitionLinkNext() override;
    void transitionFinish() override;
    void beforeTransition() override;

    virtual bool load() override;

protected:
    void itemGeometryChanged(QQuickItem *item, QQuickGeometryChange change, const QRectF &oldGeometry) override;
    AppPreloadItem *appPreloadItem() const;

private Q_SLOTS:
    void _q_onPreloadCreated(QObject *obj, const QUrl &);
    void _q_onPreloadItemVisibleChanged();
    void _q_onOverlayExitWhenChanged(bool changed);

    bool createSurface();
    void createOverlay();
    void clearOverlay();
    void doOverlayAutoExitChanged(AppPreloadItem *attached);
    void findContainerItem();
    void findWindowContentItem();

private:
    AppStartupPreloadInterface *preloadInstance = nullptr;

    QQuickItem *loadingOverlay = nullptr;
    bool overlayUsingParentSize = false;
    QVariantHash initialPropertiesHash;
};

#endif // APPSTARTUPPRELOADMODULE_H
