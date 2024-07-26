QT += core gui quick
TARGET = TokLive

CONFIG += c++17
DEFINES += QT_DEPRECATED_WARNINGS

DEFINES += CC_STARTUP_PLUGIN_PATH=\\\"$$CC_TOP_SHADOWED_DIR/bin\\\"
INCLUDEPATH += \
    $$CC_TOP_SOURCE_DIR/src \

SOURCES += \
    $$PWD/main.cpp

LIBS += -L$$CC_TOP_SHADOWED_DIR/bin -lCCQuickStartup
RESOURCES += \
    $$PWD/style.qrc      \

RC_ICONS = live.ico
