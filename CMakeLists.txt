cmake_minimum_required(VERSION 3.16)
project(WebCrawlerApp VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_BUILD_RPATH_USE_ORIGIN ON)

set(QML_IMPORT_PATH "${CMAKE_BINARY_DIR}" CACHE PATH "QML import path" FORCE)

find_package(Qt6 REQUIRED COMPONENTS
    Core
    Network
    Gui
    Qml
    Quick
)

if(COMMAND qt_policy)
    qt_policy(SET QTP0001 NEW)
    qt_policy(SET QTP0004 NEW)
endif()

add_subdirectory(src)

option(BUILD_TESTING "Build unit tests" ON)
if(BUILD_TESTING)
    find_package(GTest QUIET)
    if(GTest_FOUND)
        enable_testing()
        add_subdirectory(tests)
    endif()
endif()
