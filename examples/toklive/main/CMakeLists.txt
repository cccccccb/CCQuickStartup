qt_add_executable(TokLive WIN32 MACOSX_BUNDLE
    main.cpp
)
target_include_directories(TokLive PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)

target_compile_definitions(TokLive PRIVATE
    QT_DEPRECATED_WARNINGS
)

target_link_libraries(TokLive PRIVATE
    CCQuickStartup
    Qt::Core
    Qt::Gui
    Qt::Quick
)

# Resources:
set(style_resource_files
    "qtquickcontrols2.conf"
)

qt_add_resources(TokLive "style"
    PREFIX
        "/"
    FILES
        ${style_resource_files}
)

install(TARGETS TokLive
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_generate_deploy_app_script(
    TARGET TokLive
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
