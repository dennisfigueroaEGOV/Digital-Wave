cmake_minimum_required(VERSION 3.10)
project(Question2)

set(CMAKE_CXX_STANDARD 11)

find_package(CURL REQUIRED)

add_executable(question2 main.cpp)

target_include_directories(question2 PRIVATE ${CURL_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(question2 PRIVATE ${CURL_LIBRARIES})
