set(DEPENDES_SOURCE
    impl/edgetoedgemodehelper.cpp
    impl/edgetoedgemodehelper.h
    impl/frameless.cpp
    impl/frameless.h
    impl/framelessattached.cpp
    impl/framelessattached.h
    impl/framelessworker.cpp
    impl/framelessworker.h
    impl/framelessworkerevent.cpp
    impl/framelessworkerevent.h
    impl/iconimageprovider.cpp
    impl/iconimageprovider.h
    impl/tokliveqmlinstance.cpp
    impl/tokliveqmlinstance.h
    toklive_plugin.cpp
    toklive_plugin.h
)

set(REGISTER_QML
    modules/Controls/CheckedAnimateButton.qml
    modules/Controls/CustomButton.qml
    modules/Controls/LineEdit.qml
    modules/Controls/SearchEdit.qml
    modules/Controls/TokIcon.qml
    modules/Style/Style.qml
)

set(PRIVATE_QML
    modules/Style/desktop/CustomButtonStyle.qml
    modules/Style/desktop/DesktopStyle.qml
    modules/Style/desktop/SearchEditStyle.qml
    modules/Style/mobile/CustomButtonStyle.qml
    modules/Style/mobile/MobileStyle.qml
    modules/Style/mobile/SearchEditStyle.qml
)

set(QML_DIR_FILE
    qmldir
)

set(PLUGIN_BINARY_PATH
    ${CMAKE_BINARY_DIR}/org/orange/toklive
)

qt_add_plugin(TokLiveControlExtension
    SHARED
)

qt_add_resources(TokLiveControlExtension "qml"
    PREFIX
        "/org/orange/toklive/"
    FILES
        ${REGISTER_QML}
        ${PRIVATE_QML}
        ${QML_DIR_FILE}
)

file(COPY ${QML_DIR_FILE} DESTINATION ${PLUGIN_BINARY_PATH})

target_sources(TokLiveControlExtension PRIVATE
    ${DEPENDES_SOURCE}
)

set_target_properties(TokLiveControlExtension
    PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${PLUGIN_BINARY_PATH}
)

target_link_libraries(TokLiveControlExtension PRIVATE
    Qt::Core
    Qt::Gui
    Qt::Qml
    Qt::Quick
)

target_include_directories(TokLiveControlExtension PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/impl
)

install(TARGETS TokLiveControlExtension
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    FRAMEWORK DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
