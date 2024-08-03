TEMPLATE = lib
TARGET = CCQuickStartupExtension
QT += qml quick
CONFIG += plugin c++11 qtquickcompiler

TARGET = $$qtLibraryTarget($$TARGET)

uri = org.orange.quickstartup
DESTDIR = $$CC_TOP_SHADOWED_DIR/bin/$$replace(uri, \., /)

SOURCES += \
        $$PWD/ccquickstartup_plugin.cpp

HEADERS += \
        $$PWD/ccquickstartup_plugin.h

INCLUDEPATH += \
    $$CC_TOP_SOURCE_DIR/src

DISTFILES = qmldir
LIBS += -L$$CC_TOP_SHADOWED_DIR/bin -lCCQuickStartup

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
unix {
    installPath = $$[QT_INSTALL_QML]/$$replace(uri, \., /)
    qmldir.path = $$installPath
    target.path = $$installPath
    INSTALLS += target qmldir
}
