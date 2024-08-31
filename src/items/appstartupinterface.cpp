#include "appstartupinterface.h"

class AppStartupInterfacePrivate
{
public:
    AppStartupInterfacePrivate(AppStartupInterface *qq)
        : _qq(qq)
    {}

    AppStartupInterface *_qq;
    int _type;
};

AppStartupInterface::AppStartupInterface(QObject *parent)
    : QQmlComponent(parent)
    , dd(new AppStartupInterfacePrivate(this))
{

}

AppStartupInterface::~AppStartupInterface()
{

}

int AppStartupInterface::type() const
{
    return dd->_type;
}

void AppStartupInterface::setType(int yype)
{
    if (dd->_type == yype)
        return;
    dd->_type = yype;
    Q_EMIT typeChanged();
}
