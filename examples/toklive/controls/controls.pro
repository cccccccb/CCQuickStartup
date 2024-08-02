TEMPLATE = lib
TARGET = TokLivePlugin
QT += qml quick
CONFIG += plugin c++11 qtquickcompiler

uri = org.orange.toklive

TARGET = $$qtLibraryTarget($$TARGET)
DESTDIR = $$CC_TOP_SHADOWED_DIR/bin/$$replace(uri, \., /)

include($$PWD/impl/impl.pri)

SOURCES += \
    toklive_plugin.cpp

HEADERS += \
    toklive_plugin.h

DISTFILES = qmldir

RESOURCES += \
    modules/modules.qrc

!equals(_PRO_FILE_PWD_, $$DESTDIR) {
    _OUT_PWD_SYSTEM_DIR=$$replace(DESTDIR, /, $$QMAKE_DIR_SEP)
    _PRO_FILE_PWD_SYSTEM_DIR=$$replace(_PRO_FILE_PWD_, /, $$QMAKE_DIR_SEP)

    copy_qmldir.target = $${_OUT_PWD_SYSTEM_DIR}$${QMAKE_DIR_SEP}qmldir
    copy_qmldir.depends = $${_PRO_FILE_PWD_SYSTEM_DIR}$${QMAKE_DIR_SEP}qmldir
    copy_qmldir.commands = $(COPY_FILE) "$$replace(copy_qmldir.depends, /, $$QMAKE_DIR_SEP)" "$$replace(copy_qmldir.target, /, $$QMAKE_DIR_SEP)"
    QMAKE_EXTRA_TARGETS += copy_qmldir
    PRE_TARGETDEPS += $$copy_qmldir.target
}

qmldir.files = qmldir
