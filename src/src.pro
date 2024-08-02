TEMPLATE = lib
TARGET = CCQuickStartup

QT += quick quick-private
CONFIG += c++17 qtquickcompiler

DEFINES += CC_QUICKSTARTUP_LIBRARY
DESTDIR = $$CC_TOP_SHADOWED_DIR/bin

include(items/items.pri)
include(private/private.pri)

SOURCES += \
    appstartupinstance.cpp \
    ccstartupmainwindowinterface.cpp \
    ccstartuppreloadinterface.cpp

HEADERS += \
    appstartupinstance.h \
    ccquickstartup_global.h \
    ccstartupmainwindowinterface.h \
    ccstartuppreloadinterface.h

RESOURCES += \
    qml.qrc

