#ifndef APPSTARTUPMODULEINFORMATION_H
#define APPSTARTUPMODULEINFORMATION_H

#include "ccquickstartup_global.h"

#include <QQmlComponent>

class CC_QUICKSTARTUP_EXPORT AppStartupModuleInformation
{
    Q_GADGET
    Q_PROPERTY(StartModule startModule READ startModule CONSTANT FINAL)
    Q_PROPERTY(QString appId READ appId CONSTANT FINAL)
    Q_PROPERTY(QString descriptor READ descriptor CONSTANT FINAL)
    Q_PROPERTY(QString version READ version CONSTANT FINAL)
    Q_PROPERTY(QString description READ description CONSTANT FINAL)
    Q_PROPERTY(QStringList features READ features CONSTANT FINAL)
    Q_PROPERTY(QString changelog READ changelog CONSTANT FINAL)
    Q_PROPERTY(bool isDefault READ isDefault CONSTANT FINAL)
    Q_PROPERTY(QString path READ path CONSTANT FINAL)

public:
    AppStartupModuleInformation();
    AppStartupModuleInformation(const QString &path);
    AppStartupModuleInformation(const AppStartupModuleInformation &other);
    AppStartupModuleInformation(AppStartupModuleInformation &other);
    AppStartupModuleInformation(AppStartupModuleInformation &&other);
    AppStartupModuleInformation &operator=(const AppStartupModuleInformation &other);
    AppStartupModuleInformation &operator=(AppStartupModuleInformation &&other);
    bool operator==(const AppStartupModuleInformation &other) const;
    bool operator!=(const AppStartupModuleInformation &other) const;

    enum StartModule {
        UnknowModule = -1,
        Preload,
        Entity
    };
    Q_ENUM(StartModule)

    bool isValid() const;

    StartModule startModule() const;
    void setStartModule(StartModule module);

    QString appId() const;
    void setAppId(const QString &appId);

    QString descriptor() const;
    void setDescriptor(const QString &descriptor);

    QString version() const;
    void setVersion(const QString &version);

    QString description() const;
    void setDescription(const QString &description);

    QStringList features() const;
    void setFeatures(const QStringList &features);

    QString changelog() const;
    void setChangelog(const QString &changelog);

    bool isDefault() const;
    void setDefault(bool isDefault);

    QString path() const;
    void setPath(const QString &path);

private:
    StartModule m_startModule;
    QString m_appId;
    QString m_descriptor;
    QString m_version;
    QString m_description;
    QStringList m_features;
    QString m_changelog;
    bool m_default;
    QString m_path;
};

inline size_t qHash(const AppStartupModuleInformation &key, size_t seed = 0) {
    return qHash(key.appId(), seed) ^ qHash(key.descriptor(), seed) ^
           qHash(key.version(), seed) ^ qHash(static_cast<int>(key.startModule()), seed);
}

class QQuickItem;

class CC_QUICKSTARTUP_EXPORT AppStartupModuleGroup
{
    Q_GADGET
    Q_PROPERTY(AppStartupModuleInformation preload READ preload CONSTANT FINAL)
    Q_PROPERTY(AppStartupModuleInformation entity READ entity CONSTANT FINAL)
    Q_PROPERTY(bool loaded READ loaded CONSTANT FINAL)

public:
    AppStartupModuleGroup();
    AppStartupModuleGroup(std::pair<AppStartupModuleInformation, AppStartupModuleInformation> args);
    AppStartupModuleGroup(const AppStartupModuleGroup &other);
    AppStartupModuleGroup(AppStartupModuleGroup &&other);
    AppStartupModuleGroup &operator=(const AppStartupModuleGroup &other);
    AppStartupModuleGroup &operator=(AppStartupModuleGroup &&other);
    bool operator==(const AppStartupModuleGroup &other) const;
    bool operator!=(const AppStartupModuleGroup &other) const;

    bool isValid() const;

    AppStartupModuleInformation preload() const;
    void setPreload(const AppStartupModuleInformation &preload);

    AppStartupModuleInformation entity() const;
    void setEntity(const AppStartupModuleInformation &entity);

    bool loaded() const;
    Q_INVOKABLE void setItemSurface(QQuickItem *item);

private:
    friend class AppStartupPreloadModule;
    enum BindingProperty {
        ItemSurface
    };
    QVariant bindingProperty(BindingProperty property) const;

private:
    typedef std::pair<AppStartupModuleInformation, AppStartupModuleInformation> Group;
    Group m_group;
    QMap<BindingProperty, QVariant> m_bindingProperties;
};

inline size_t qHash(const AppStartupModuleGroup &key, size_t seed = 0) {
    return qHash(key.preload(), seed) ^ qHash(key.entity(), seed);
}

#endif // APPSTARTUPMODULEINFORMATION_H
