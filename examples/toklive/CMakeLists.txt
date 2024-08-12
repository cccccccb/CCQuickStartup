cmake_minimum_required(VERSION 3.16)
project(toklive VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(QT NAMES Qt6 REQUIRED COMPONENTS Core)

qt_standard_project_setup()

add_subdirectory(controls)
add_subdirectory(preload)
add_subdirectory(entity)
add_subdirectory(main)
