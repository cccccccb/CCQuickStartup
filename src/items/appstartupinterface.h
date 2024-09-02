#ifndef APPSTARTUPINTERFACE_H
#define APPSTARTUPINTERFACE_H

#include "ccquickstartup_global.h"

#include <QObject>
#include <QQmlComponent>
#include <QQmlEngine>

class AppStartupInterfacePrivate;

class CC_QUICKSTARTUP_EXPORT AppStartupInterface : public QQmlComponent
{
    Q_OBJECT
    Q_PROPERTY(int type READ type WRITE setType NOTIFY typeChanged FINAL)
    Q_PROPERTY(QQmlComponent *object READ object WRITE setObject NOTIFY objectChanged FINAL)

    Q_CLASSINFO("DefaultProperty", "object")
    QML_NAMED_ELEMENT(AppStartupInterface)

public:
    explicit AppStartupInterface(QObject *parent = nullptr);
    ~AppStartupInterface();

    int type() const;
    void setType(int newType);

    QQmlComponent *object() const;
    void setObject(QQmlComponent *object);

Q_SIGNALS:
    void typeChanged();
    void objectChanged();

private:
    QScopedPointer<AppStartupInterfacePrivate> dd;
};

#endif // APPSTARTUPINTERFACE_H
