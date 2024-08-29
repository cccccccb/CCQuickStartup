qt_add_plugin(TokLive-DesktopPreload
    SHARED
    toklive_preload.cpp toklive_preload.h
)

set_target_properties(TokLive-DesktopPreload
    PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/plugins
)

target_link_libraries(TokLive-DesktopPreload PRIVATE
    CCQuickStartup
    # TokLiveControlExtension
    Qt::Core
    Qt::Gui
    Qt::Quick
)

# Resources:
set(qml_resource_files
    "qml/Countdown.qml"
    "qml/PreloadOverlay.qml"
    "qml/PreloadWindow.qml"
)

qt_add_resources(TokLive-DesktopPreload "qml"
    PREFIX
        "/"
    FILES
        ${qml_resource_files}
)

set(res_resource_files
    "res/live.png"
)

qt_add_resources(TokLive-DesktopPreload "res"
    PREFIX
        "/"
    BASE
        "res"
    FILES
        ${res_resource_files}
)

install(TARGETS TokLive-DesktopPreload
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    FRAMEWORK DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
