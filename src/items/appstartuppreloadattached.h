#ifndef APPSTARTUPPRELOADATTACHED_H
#define APPSTARTUPPRELOADATTACHED_H

#include <QObject>
#include <QQmlComponent>

#include "ccquickstartup_global.h"

QT_BEGIN_NAMESPACE
class QQuickTransition;
QT_END_NAMESPACE

class AppStartupItem;
class AppStartupPreloadAttachedPrivate;

class CC_QUICKSTARTUP_EXPORT AppStartupPreloadAttached : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QQmlComponent *transitionGroup READ transitionGroup WRITE setTransitionGroup NOTIFY transitionGroupChanged FINAL)
    Q_PROPERTY(QQmlComponent *loadingOverlay READ loadingOverlay WRITE setLoadingOverlay NOTIFY loadingOverlayChanged FINAL)
    Q_PROPERTY(bool autoExitOverlay READ autoExitOverlay WRITE setAutoExitOverlay NOTIFY autoExitOverlayChanged FINAL)
    Q_PROPERTY(bool overlayExitWhen READ overlayExitWhen WRITE setOverlayExitWhen NOTIFY overlayExitWhenChanged FINAL)
    Q_PROPERTY(AppStartupItem *startupItem READ startupItem NOTIFY startupItemChanged FINAL)
    Q_PROPERTY(bool loaded READ loaded NOTIFY loadedChanged FINAL)

    QML_ANONYMOUS
    QML_UNCREATABLE("AppStartupPreloadAttached Attached.")
    QML_NAMED_ELEMENT(PreloadItem)
    QML_ATTACHED(AppStartupPreloadAttached)

public:
    explicit AppStartupPreloadAttached(QObject *parent = nullptr);
    ~AppStartupPreloadAttached();

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

    static AppStartupPreloadAttached *qmlAttachedProperties(QObject *object);

Q_SIGNALS:
    void transitionGroupChanged();
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
