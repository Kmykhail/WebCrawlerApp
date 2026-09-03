add_executable(tests
    test_crawlermanager.cpp
)

target_include_directories(tests
    PRIVATE
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/src/core
)

target_link_libraries(tests
    PRIVATE
        project_core
        Qt6::Core
        Qt6::Network
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(tests)
