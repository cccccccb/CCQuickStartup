#ifndef APPSTARTUPPRELOADATTACHED_H
#define APPSTARTUPPRELOADATTACHED_H

#include <QObject>
#include <QQmlComponent>

#include "ccquickstartup_global.h"

QT_BEGIN_NAMESPACE
class QQuickTransition;
QT_END_NAMESPACE

class QuickAppStartupItem;
class AppStartupPreloadAttachedPrivate;

class CC_QUICKSTARTUP_EXPORT AppStartupPreloadAttached : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QQuickTransition *overlayExited READ overlayExited WRITE setOverlayExited NOTIFY overlayExitedChanged FINAL)
    Q_PROPERTY(QQmlComponent *loadingOverlay READ loadingOverlay WRITE setLoadingOverlay NOTIFY loadingOverlayChanged FINAL)
    Q_PROPERTY(bool autoExitOverlay READ autoExitOverlay WRITE setAutoExitOverlay NOTIFY autoExitOverlayChanged FINAL)
    Q_PROPERTY(bool overlayExitWhen READ overlayExitWhen WRITE setOverlayExitWhen NOTIFY overlayExitWhenChanged FINAL)
    Q_PROPERTY(QuickAppStartupItem *startupItem READ startupItem NOTIFY startupItemChanged FINAL)
    Q_PROPERTY(bool loaded READ loaded NOTIFY loadedChanged FINAL)

    QML_ANONYMOUS
    QML_UNCREATABLE("AppStartupPreloadAttached Attached.")
    QML_NAMED_ELEMENT(PreloadItem)
    QML_ATTACHED(AppStartupPreloadAttached)

public:
    explicit AppStartupPreloadAttached(QObject *parent = nullptr);
    ~AppStartupPreloadAttached();

    QQuickTransition *overlayExited() const;
    void setOverlayExited(QQuickTransition *exit);

    QQmlComponent *loadingOverlay() const;
    void setLoadingOverlay(QQmlComponent *component);

    bool autoExitOverlay() const;
    void setAutoExitOverlay(bool autoExitOverlay);

    bool overlayExitWhen() const;
    void setOverlayExitWhen(bool overlayExitWhen);

    QuickAppStartupItem *startupItem() const;
    void setStartupItem(QuickAppStartupItem *item);

    bool loaded() const;
    void setLoaded(bool loaded);

    static AppStartupPreloadAttached *qmlAttachedProperties(QObject *object);

Q_SIGNALS:
    void overlayExitedChanged();
    void loadingOverlayChanged();
    void autoExitOverlayChanged(bool changed);
    void overlayExitWhenChanged(bool changed);
    void startupItemChanged();
    void loadedChanged();

private:
    QScopedPointer<AppStartupPreloadAttachedPrivate> dd;
};

QML_DECLARE_TYPEINFO(AppStartupPreloadAttached, QML_HAS_ATTACHED_PROPERTIES)

#endif // APPSTARTUPPRELOADATTACHED_H
