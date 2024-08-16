#ifndef APPSTARTUPCOMPONENTINFORMATION_H
#define APPSTARTUPCOMPONENTINFORMATION_H

#include "ccquickstartup_global.h"

#include <QQmlComponent>

class CC_QUICKSTARTUP_EXPORT AppStartupComponentInformation
{
    Q_GADGET
    Q_PROPERTY(StartComponent startComponent READ startComponent CONSTANT FINAL)
    Q_PROPERTY(QString appId READ appId CONSTANT FINAL)
    Q_PROPERTY(QString descriptor READ descriptor CONSTANT FINAL)
    Q_PROPERTY(QString version READ version CONSTANT FINAL)
    Q_PROPERTY(QString description READ description CONSTANT FINAL)
    Q_PROPERTY(QStringList features READ features CONSTANT FINAL)
    Q_PROPERTY(QString changelog READ changelog CONSTANT FINAL)
    Q_PROPERTY(bool isDefault READ isDefault CONSTANT FINAL)
    Q_PROPERTY(QString path READ path CONSTANT FINAL)
    QML_ANONYMOUS

public:
    AppStartupComponentInformation();
    AppStartupComponentInformation(const AppStartupComponentInformation &other);
    AppStartupComponentInformation(AppStartupComponentInformation &&other);
    AppStartupComponentInformation &operator=(const AppStartupComponentInformation &other);
    AppStartupComponentInformation &operator=(AppStartupComponentInformation &&other);
    bool operator==(const AppStartupComponentInformation &other) const;
    bool operator!=(const AppStartupComponentInformation &other) const;

    enum StartComponent {
        UnknowComponent = -1,
        Preload,
        Entity
    };
    Q_ENUM(StartComponent)

    bool isValid() const;

    StartComponent startComponent() const;
    void setStartComponent(StartComponent component);

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
    StartComponent m_startComponent;
    QString m_appId;
    QString m_descriptor;
    QString m_version;
    QString m_description;
    QStringList m_features;
    QString m_changelog;
    bool m_default;
    QString m_path;
};

inline size_t qHash(const AppStartupComponentInformation &key, size_t seed = 0) {
    return qHash(key.appId(), seed)
           ^ qHash(key.descriptor(), seed)
           ^ qHash(key.version(), seed)
           ^ qHash(static_cast<int>(key.startComponent()), seed);
}

class CC_QUICKSTARTUP_EXPORT AppStartupComponentGroup
{
    Q_GADGET
    Q_PROPERTY(AppStartupComponentInformation preload READ preload CONSTANT FINAL)
    Q_PROPERTY(AppStartupComponentInformation entity READ entity CONSTANT FINAL)
    QML_ANONYMOUS

public:
    AppStartupComponentGroup();
    AppStartupComponentGroup(std::pair<AppStartupComponentInformation, AppStartupComponentInformation> args);
    AppStartupComponentGroup(const AppStartupComponentGroup &other);
    AppStartupComponentGroup(AppStartupComponentGroup &&other);
    AppStartupComponentGroup &operator=(const AppStartupComponentGroup &other);
    AppStartupComponentGroup &operator=(AppStartupComponentGroup &&other);
    bool operator==(const AppStartupComponentGroup &other) const;
    bool operator!=(const AppStartupComponentGroup &other) const;

    bool isValid() const;

    AppStartupComponentInformation preload() const;
    void setPreload(const AppStartupComponentInformation &preload);

    AppStartupComponentInformation entity() const;
    void setEntity(const AppStartupComponentInformation &entity);

private:
    typedef std::pair<AppStartupComponentInformation, AppStartupComponentInformation> Group;
    Group m_group;
};

inline size_t qHash(const AppStartupComponentGroup &key, size_t seed = 0) {
    return qHash(key.preload(), seed)
           ^ qHash(key.entity(), seed);
}

#endif // APPSTARTUPCOMPONENTINFORMATION_H
