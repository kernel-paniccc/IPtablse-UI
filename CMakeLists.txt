cmake_minimum_required(VERSION 3.16)

project(IptablesUiDemo VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

qt_add_executable(iptables-ui
    src/main.cpp
    src/mainwindow.cpp
    src/mainwindow.h
    src/iptablesrule.h
    src/iptablesloader.cpp
    src/iptablesloader.h
    src/networkmonitor.cpp
    src/networkmonitor.h
    src/trafficwidget.cpp
    src/trafficwidget.h
)

target_link_libraries(iptables-ui PRIVATE Qt6::Widgets)

