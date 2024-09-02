#ifndef APPSTARTUPPRELOADMODULEOBJECT_H
#define APPSTARTUPPRELOADMODULEOBJECT_H

#include "appstartupmoduleobject.h"

#include "items/appstartuppreloaditem.h"

class AppStartupPreloadInterface;

class AppStartupPreloadModuleObject : public AppStartupModuleObject
{
    Q_OBJECT
public:
    using AppStartupModuleObject::AppStartupModuleObject;
    ~AppStartupPreloadModuleObject();

    inline AppStartupModuleInformation::StartModule moduleType() override { return AppStartupModuleInformation::Preload; }

    inline QQuickItem *transitionItem() override { return loadingOverlay; }
    virtual QQuickTransition *transition() override;

    AppStartupModuleObject *transitionLinkNext() override;
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

#endif // APPSTARTUPPRELOADMODULEOBJECT_H
