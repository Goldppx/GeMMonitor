cmake_minimum_required(VERSION 3.31.0)

project(GeMMonitor VERSION 0.1.0)
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CPACK_PACKAGE_NAME "GeMMonitor")
set(CPACK_PACKAGE_VENDOR "GoldesqueMoON")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "A simple system monitor for Arch Linux and Ubuntu")
set(CPACK_STRIP_FILES TRUE)

add_definitions(-DAPP_VERSION="${PROJECT_VERSION}")

find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        REQUIRED)

add_executable(GeMMonitor main.cpp
        GeMMonitor.cpp
        GeMMonitor.ui
        GeMMonitor.h)


target_link_libraries(GeMMonitor
        Qt::Core
        Qt::Gui
        Qt::Widgets
)

set(CPACK_DEBIAN_PACKAGE_MAINTAINER "GoldesqueMoON")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libqt6widgets6, libqt6gui6, libqt6core6")

if(UNIX AND NOT APPLE)
    set(CPACK_GENERATOR "DEB;TXZ")
endif()

include(CPack)
