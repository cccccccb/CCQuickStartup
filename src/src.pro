TEMPLATE = lib
TARGET = CCQuickStartup

QT += quick quick-private
CONFIG += c++17

DEFINES += CC_QUICKSTARTUP_LIBRARY
DESTDIR = $$CC_TOP_SHADOWED_DIR/bin

SOURCES += \
    appstartupcomponentinformation.cpp \
    appstartupinstance.cpp \
    appstartupinstanceattached.cpp \
    appstartuppreloadattached.cpp \
    ccquickstartupmainwindowinterface.cpp \
    ccquickstartuppreloadinterface.cpp \
    quickappstartupitem.cpp

HEADERS += \
    appstartupcomponentinformation.h \
    appstartupinstance.h \
    appstartupinstanceattached.h \
    appstartuppreloadattached.h \
    ccquickstartup_global.h \
    ccquickstartupmainwindowinterface.h \
    ccquickstartuppreloadinterface.h \
    quickappstartupitem.h

