TEMPLATE = lib
TARGET = TokLive-DesktopPreload

QT += quick

CONFIG += c++17 plugin qtquickcompiler
DESTDIR = $$CC_TOP_SHADOWED_DIR/bin

QML_IMPORT_PATH += $${CC_TOP_SHADOWED_DIR}/bin/
DEFINES += CC_STARTUP_PLUGIN_PATH=\\\"$$CC_TOP_SHADOWED_DIR/bin\\\"

INCLUDEPATH += \
    $$CC_TOP_SOURCE_DIR/src \

LIBS += -L$$CC_TOP_SHADOWED_DIR/bin -lCCQuickStartup

HEADERS += \
    toklivepreload.h

SOURCES += \
    toklivepreload.cpp

RESOURCES += \
    $$PWD/qml.qrc   \
    $$PWD/res/res.qrc

DISTFILES += \
    plugin.json
