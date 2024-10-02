cmake_minimum_required(VERSION 3.10)
project(untitled)

set(CMAKE_CXX_STANDARD 11)

find_package(CURL REQUIRED)

add_executable(untitled main.cpp
)

target_include_directories(untitled PRIVATE ${CURL_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(untitled PRIVATE ${CURL_LIBRARIES})