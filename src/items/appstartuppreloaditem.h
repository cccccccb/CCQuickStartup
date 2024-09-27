#ifndef APPSTARTUPPRELOADITEM_H
#define APPSTARTUPPRELOADITEM_H

#include <QQuickItem>

QT_BEGIN_NAMESPACE
class QQmlComponent;
class QQuickWindow;
QT_END_NAMESPACE

#include "ccquickstartup_global.h"

class AppStartupItem;
class AppPreloadItemPrivate;
class AppStartupInitialProperties;

class CC_QUICKSTARTUP_EXPORT AppPreloadItem : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(PreloadSurface preloadSurface READ preloadSurface WRITE setPreloadSurface NOTIFY preloadSurfaceChanged FINAL)
    Q_PROPERTY(QQmlComponent *customPreloadSurface READ customPreloadSurface WRITE setCustomPreloadSurface NOTIFY customPreloadSurfaceChanged FINAL)
    Q_PROPERTY(QQmlComponent *transitionGroup READ transitionGroup WRITE setTransitionGroup NOTIFY transitionGroupChanged FINAL)
    Q_PROPERTY(QQmlComponent *loadingOverlay READ loadingOverlay WRITE setLoadingOverlay NOTIFY loadingOverlayChanged FINAL)
    Q_PROPERTY(bool autoExitOverlay READ autoExitOverlay WRITE setAutoExitOverlay NOTIFY autoExitOverlayChanged FINAL)
    Q_PROPERTY(bool overlayExitWhen READ overlayExitWhen WRITE setOverlayExitWhen NOTIFY overlayExitWhenChanged FINAL)
    Q_PROPERTY(AppStartupItem *startupItem READ startupItem NOTIFY startupItemChanged FINAL)
    Q_PROPERTY(bool loaded READ loaded NOTIFY loadedChanged FINAL)
    Q_PROPERTY(AppStartupInitialProperties *initialProperties READ initialProperties WRITE setInitialProperties NOTIFY initialPropertiesChanged FINAL)
    Q_PROPERTY(QQuickWindow *window READ window NOTIFY windowChanged FINAL)
    QML_NAMED_ELEMENT(AppPreloadItem)

public:
    explicit AppPreloadItem(QQuickItem *parentItem = nullptr);
    ~AppPreloadItem();

    enum PreloadSurface {
        WindowSurface,
        ApplicationWindowSurface,
        ItemSurface,
        CustomSurface
    };
    Q_ENUM(PreloadSurface)

    PreloadSurface preloadSurface() const;
    void setPreloadSurface(PreloadSurface surface);

    QQmlComponent *customPreloadSurface() const;
    void setCustomPreloadSurface(QQmlComponent *custom);

    QQmlComponent *transitionGroup() const;
    void setTransitionGroup(QQmlComponent *transitionGroup);

    QQmlComponent *loadingOverlay() const;
    void setLoadingOverlay(QQmlComponent *component);

    bool autoExitOverlay() const;
    void setAutoExitOverlay(bool autoExitOverlay);

    bool overlayExitWhen() const;
    void setOverlayExitWhen(bool overlayExitWhen);

    AppStartupItem *startupItem() const;
    void setStartupItem(AppStartupItem *item);

    bool loaded() const;
    void setLoaded(bool loaded);

    AppStartupInitialProperties *initialProperties() const;
    void setInitialProperties(AppStartupInitialProperties *initialProperties);

    void setWindow(QQuickWindow *window);
    QQuickWindow *window() const;

Q_SIGNALS:
    void preloadSurfaceChanged();
    void customPreloadSurfaceChanged();
    void transitionGroupChanged();
    void loadingOverlayChanged();
    void autoExitOverlayChanged(bool changed);
    void overlayExitWhenChanged(bool changed);
    void startupItemChanged();
    void loadedChanged();
    void initialPropertiesChanged();
    void windowChanged();

private:
    friend class AppStartupPreloadModuleObject;
    QScopedPointer<AppPreloadItemPrivate> dd;
};

#endif // APPSTARTUPPRELOADITEM_H
