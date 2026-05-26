#ifndef APPSTARTUPMODULEGROUP_H
#define APPSTARTUPMODULEGROUP_H

#include "ccquickstartup_global.h"
#include "appstartupmoduleinformation.h"

#include <QObject>

class QQuickItem;
class AppStartupInterface;
class AppStartupModuleGroupPrivate;

class CC_QUICKSTARTUP_EXPORT AppStartupModuleGroup : public QObject
{
    Q_OBJECT
    Q_PROPERTY(AppStartupModuleInformation preload READ preload CONSTANT FINAL)
    Q_PROPERTY(AppStartupModuleInformation entity READ entity CONSTANT FINAL)

    Q_PROPERTY(Status status READ status NOTIFY statusChanged FINAL)
    Q_PROPERTY(QString errorString READ errorString NOTIFY errorStringChanged FINAL)

    Q_PROPERTY(bool loaded READ loaded NOTIFY loadedChanged FINAL)

public:
    enum Status {
        Idle,       // 初始状态
        Loading,    // 正在加载 (Preload 或 Entity)
        Ready,      // 加载完成
        Error       // 发生错误
    };
    Q_ENUM(Status)

    explicit AppStartupModuleGroup(QObject *parent = nullptr);
    AppStartupModuleGroup(std::pair<AppStartupModuleInformation, AppStartupModuleInformation> args, QObject *parent = nullptr);
    ~AppStartupModuleGroup();
    bool operator==(const AppStartupModuleGroup &other) const;
    bool operator!=(const AppStartupModuleGroup &other) const;

    bool isValid() const;

    AppStartupModuleInformation preload() const;
    void setPreload(const AppStartupModuleInformation &preload);

    AppStartupModuleInformation entity() const;
    void setEntity(const AppStartupModuleInformation &entity);

    Status status() const;
    void setStatus(Status status);

    QString errorString() const;
    void setError(const QString &errorString);

    bool loaded() const;

    void setSurfaceItem(QQuickItem *item);
    QList<AppStartupInterface *> resolveInterface(int type);

    enum BindingProperty {
        SurfaceItem,
        ResolvedInterface
    };

    static QList<QSharedPointer<AppStartupModuleGroup>> loadFromPath(const QString &path);

Q_SIGNALS:
    void loadedChanged();
    void statusChanged();
    void errorStringChanged();
    void errorOccured(const QString &errorString);

protected:
    QVariant bindingProperty(BindingProperty property) const;
    void insertBindingProperty(BindingProperty property, QVariant value);
    void insertBindingPropertyList(BindingProperty property, QVariantList value);

private:
    friend class AppStartupModuleObject;
    friend class AppStartupModuleGroupPrivate;
    Q_DISABLE_COPY_MOVE(AppStartupModuleGroup)
    QScopedPointer<AppStartupModuleGroupPrivate> dd;
};

#endif // APPSTARTUPMODULEGROUP_H
