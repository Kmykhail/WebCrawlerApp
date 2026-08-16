cmake_minimum_required(VERSION 3.16)

project(WebCrawlerApp VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(GTest REQUIRED)

enable_testing()

add_executable(tests
    test_crawlermanager.cpp
)

target_include_directories(tests PRIVATE ${CMAKE_SOURCE_DIR})

target_link_libraries(tests
    PRIVATE webcrawler_lib Qt6::Core Qt6::Network GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(tests)
