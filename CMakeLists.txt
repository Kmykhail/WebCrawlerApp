cmake_minimum_required(VERSION 3.16)

project(WebCrawlerApp VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Quick Network Core)

qt_standard_project_setup(REQUIRES 6.8)

add_library(webcrawler_lib
    crawlermanager.cpp
    worker.cpp
    model/UrlModel.cpp
)

target_link_libraries(webcrawler_lib
    PRIVATE Qt6::Core Qt6::Network
)

qt_add_executable(appWebCrawlerApp
    main.cpp
)

target_link_libraries(appWebCrawlerApp
    PRIVATE webcrawler_lib Qt6::Quick
)

qt_add_qml_module(appWebCrawlerApp
    URI WebCrawlerApp
    VERSION 1.0
    QML_FILES
        Main.qml
)

# Define QT_NO_DEBUG_OUTPUT for Release builds
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DQT_NO_DEBUG_OUTPUT")

include(GNUInstallDirs)
install(TARGETS appWebCrawlerApp
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

add_subdirectory(tests)
