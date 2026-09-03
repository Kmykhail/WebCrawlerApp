qt_add_qml_module(ui_module
    URI WebCrawlerApp
    VERSION 1.0
    RESOURCE_PREFIX "/qt/qml"
    OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/WebCrawlerApp"
    SOURCES
        ${CMAKE_SOURCE_DIR}/src/controller/crawlercontroller.h
        ${CMAKE_SOURCE_DIR}/src/controller/crawlercontroller.cpp
        logger/logcontroller.h
        logger/logcontroller.cpp
        models/logmodel.h
        models/logmodel.cpp
        models/urlmodel.h
        models/urlmodel.cpp

    QML_FILES
        Main.qml
        qml/components/ConsolPanel.qml
        qml/components/ControlPanel.qml
        qml/components/HeaderBar.qml
        qml/components/InfoPanel.qml
        qml/components/UrlTableView.qml
)

target_link_libraries(ui_module
    PRIVATE
        project_core
        Qt6::Quick
        Qt6::Gui
)

target_include_directories(ui_module
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/models
        ${CMAKE_CURRENT_SOURCE_DIR}/logger
        ${CMAKE_SOURCE_DIR}/src/controller
)
