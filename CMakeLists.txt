cmake_minimum_required(VERSION 3.31.0)
project(GeMMonitor)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

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
