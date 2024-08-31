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
    QML_NAMED_ELEMENT(AppStartupInterface)

public:
    explicit AppStartupInterface(QObject *parent = nullptr);
    ~AppStartupInterface();

    int type() const;
    void setType(int newType);

Q_SIGNALS:
    void typeChanged();

private:
    QScopedPointer<AppStartupInterfacePrivate> dd;
};

#endif // APPSTARTUPINTERFACE_H
