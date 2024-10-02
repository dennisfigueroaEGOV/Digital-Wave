cmake_minimum_required(VERSION 3.10)
project(Question1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(question1 main.cpp)
set(TEXT_FILE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/98-0.txt")

configure_file(${TEXT_FILE_PATH} ${CMAKE_BINARY_DIR}/98-0.txt COPYONLY)

target_compile_definitions(question1 PRIVATE TEXT_FILE_PATH="${TEXT_FILE_PATH}")
