cmake_minimum_required(VERSION 3.16)
project(CCQuickStartup VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(QT NAMES Qt6 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Gui Quick)

file(GLOB_RECURSE DEPENDS_SOUCES_H "*.h")
file(GLOB_RECURSE DEPENDS_SOUCES_CPP "*.cpp")
file(GLOB EXTENSION_SOURCE_H "items/*.h")

set(EXTENSION_SOURCE_QML
    "qml/transition/FlickTransition.qml"
)

set(EXTENSION_PRIVATE_SOURCE_QML
    "qml/container/Window.qml"
    "qml/container/ApplicationWindow.qml"
)

qt_standard_project_setup()

qt_add_library(CCQuickStartup
    SHARED
    ${DEPENDS_SOUCES_H}
    ${DEPENDS_SOUCES_CPP}
)

qt_add_qml_module(CCQuickStartup
    URI CCStartup
    VERSION ${PROJECT_VERSION}
    QML_FILES ${EXTENSION_SOURCE_QML}
    SOURCES ${EXTENSION_SOURCE_H}
    PLUGIN_TARGET CCQuickStartupExtension
    OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/CCStartup
)

qt_add_resources(
    CCQuickStartup "private_qml"
    PREFIX "/appstartup/private"
    FILES ${EXTENSION_PRIVATE_SOURCE_QML}
)

target_include_directories(CCQuickStartup PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/items
)

target_compile_definitions(CCQuickStartup PRIVATE
    CC_QUICKSTARTUP_LIBRARY
)

target_link_libraries(CCQuickStartup PUBLIC
    Qt::Core
    Qt::Gui
    Qt::Quick
    Qt::QuickPrivate
)

install(TARGETS CCQuickStartup
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    FRAMEWORK DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
