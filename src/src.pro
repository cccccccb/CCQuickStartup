TEMPLATE = lib
TARGET = CCQuickStartup

QT += quick quick-private
CONFIG += c++17

DEFINES += CC_QUICKSTARTUP_LIBRARY
DESTDIR = $$CC_TOP_SHADOWED_DIR/bin

include(items/items.pri)
include(private/private.pri)
include(interface/interface.pri)

SOURCES += \
    appstartupinstance.cpp \

HEADERS += \
    appstartupinstance.h \
    ccquickstartup_global.h \

RESOURCES += \
    qml.qrc

