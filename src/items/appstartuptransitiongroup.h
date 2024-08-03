#ifndef APPSTARTUPTRANSITIONGROUP_H
#define APPSTARTUPTRANSITIONGROUP_H

#include <QObject>
#include <QtQml>

#include "ccquickstartup_global.h"

QT_BEGIN_NAMESPACE
class QQuickTransition;
QT_END_NAMESPACE

class AppStartupItem;
class AppStartupInitialProperties;
class AppStartupTransitionGroupPrivate;
class CC_QUICKSTARTUP_EXPORT AppStartupTransitionGroup : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QQuickTransition *leave READ leave WRITE setLeave NOTIFY leaveChanged FINAL)
    Q_PROPERTY(QQuickTransition *enter READ enter WRITE setEnter NOTIFY enterChanged FINAL)
    Q_PROPERTY(AppStartupInitialProperties *leaveInitialProperties READ leaveInitialProperties WRITE setLeaveInitialProperties NOTIFY leaveInitialPropertiesChanged FINAL)
    Q_PROPERTY(AppStartupInitialProperties *enterInitialProperties READ enterInitialProperties WRITE setEnterInitialProperties NOTIFY enterInitialPropertiesChanged FINAL)
    QML_NAMED_ELEMENT(TransitionGroup)

public:
    explicit AppStartupTransitionGroup(QObject *parent = nullptr);
    ~AppStartupTransitionGroup();

    QQuickTransition *leave() const;
    void setLeave(QQuickTransition *leave);

    QQuickTransition *enter() const;
    void setEnter(QQuickTransition *enter);

    AppStartupInitialProperties *leaveInitialProperties() const;
    void setLeaveInitialProperties(AppStartupInitialProperties *itemInitialProperties);

    AppStartupInitialProperties *enterInitialProperties() const;
    void setEnterInitialProperties(AppStartupInitialProperties *itemInitialProperties);

signals:
    void leaveChanged();
    void enterChanged();
    void leaveInitialPropertiesChanged();
    void enterInitialPropertiesChanged();

private:
    QScopedPointer<AppStartupTransitionGroupPrivate> dd;
};

#endif // APPSTARTUPTRANSITIONGROUP_H
