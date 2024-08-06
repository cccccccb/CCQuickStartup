QT += core gui quick
TARGET = TokLive

CONFIG += c++17 qtquickcompiler
DEFINES += QT_DEPRECATED_WARNINGS

DEFINES += CC_STARTUP_PLUGIN_PATH=\\\"$$CC_TOP_SHADOWED_DIR/bin\\\"
LIBS += -L$$CC_TOP_SHADOWED_DIR/bin -lCCQuickStartup

INCLUDEPATH += \
    $$CC_TOP_SOURCE_DIR/src \

SOURCES += \
    $$PWD/main.cpp

RESOURCES += \
    $$PWD/style.qrc      \

RC_ICONS = live.ico
