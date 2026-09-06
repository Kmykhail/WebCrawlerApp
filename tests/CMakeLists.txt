add_executable(tests
    test_crawlermanager.cpp
    test_queuehandler.cpp
    test_urlfetcher.cpp
    test_worker.cpp
)

target_include_directories(tests
    PRIVATE
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/src/core
        ${CMAKE_SOURCE_DIR}/src/core/manager
        ${CMAKE_SOURCE_DIR}/src/core/worker
)

find_package(Qt6 REQUIRED COMPONENTS Core Network Test)

target_link_libraries(tests
    PRIVATE
        project_core
        Qt6::Core
        Qt6::Network
        Qt6::Test
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(tests)
