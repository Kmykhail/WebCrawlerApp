add_subdirectory(core)
add_subdirectory(ui)

add_executable(WebCrawlerApp
    main.cpp
)

target_link_libraries(WebCrawlerApp PRIVATE
    ui_module
    project_core
    Qt6::Gui
    Qt6::Qml
)

foreach(_target WebCrawlerApp ui_module project_core)
    target_compile_definitions(${_target} PRIVATE
        $<$<CONFIG:Release>:QT_NO_DEBUG_OUTPUT>
    )
endforeach()

qt_import_qml_plugins(WebCrawlerApp)
