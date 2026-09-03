find_package(Qt6 REQUIRED COMPONENTS Network Core)

add_library(project_core STATIC
    manager/crawlermanager.cpp
    manager/crawlermanager.h
    manager/queuehandler.cpp
    manager/queuehandler.h
    manager/urlfetcher.cpp
    manager/urlfetcher.h
    worker/worker.cpp
    worker/worker.h
    CrawlItem.h
    CrawlerState.h
    UrlData.h
)

target_include_directories(project_core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/manager
        ${CMAKE_CURRENT_SOURCE_DIR}/worker
)

target_link_libraries(project_core
    PUBLIC
        Qt6::Core
        Qt6::Network
)
