TEMPLATE = lib
TARGET = TokLive-DesktopEntity

QT += quick

CONFIG += c++17 plugin qtquickcompiler
DESTDIR = $$CC_TOP_SHADOWED_DIR/bin

QML_IMPORT_PATH += $${CC_TOP_SHADOWED_DIR}/bin/
INCLUDEPATH += \
    $$CC_TOP_SOURCE_DIR/src

LIBS += -L$$CC_TOP_SHADOWED_DIR/bin -lCCQuickStartup

INCLUDEPATH += \
    $$PWD/../../controls/impl

HEADERS += \
    toklive_entity.h

SOURCES += \
    toklive_entity.cpp

RESOURCES += \
    $$PWD/qml/qml.qrc       \
    $$PWD/res/res.qrc       \
    $$PWD/icons/icons.qrc   \

DISTFILES += \
    plugin.json
