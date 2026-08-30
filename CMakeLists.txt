cmake_minimum_required(VERSION 3.16)

project(WebCrawlerApp VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Quick Network Core)

qt_standard_project_setup(REQUIRES 6.8)

include_directories(
    src/domain
    src/domain/worker
    src/logger
    src/model/
    src/controller
)

add_library(webcrawler_lib
    src/domain/crawlermanager.cpp
    src/domain/worker/worker.cpp
    src/model/UrlModel.cpp
    src/domain/CrawlItem.h
    src/controller/crawlercontroller.cpp
    src/domain/UrlData.h
    src/domain/CrawlerState.h
    src/logger/logcontroller.cpp
    src/model/logmodel.cpp
    src/domain/LogData.h
)

target_link_libraries(webcrawler_lib
    PRIVATE Qt6::Core Qt6::Network
)

qt_add_executable(appWebCrawlerApp
    src/main.cpp
)

target_link_libraries(appWebCrawlerApp
    PRIVATE webcrawler_lib Qt6::Quick
)

qt_add_qml_module(appWebCrawlerApp
    URI WebCrawlerApp
    VERSION 1.0
    QML_FILES
        src/qml/Main.qml
        src/qml/components/HeaderBar.qml
        src/qml/components/ControlPanel.qml
        src/qml/components/UrlTableView.qml
        src/qml/components/ConsolPanel.qml
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
