cmake_minimum_required(VERSION 3.7)

project(test_lib)

find_package(Catch2 REQUIRED)

set(INC_PATH "../include")
set(LIB_PATH "../build/")

file(GLOB HEADERS ${INC_PATH}/*.h)

include_directories(${INC_PATH})
link_directories(${PROJECT_NAME} ${LIB_PATH})

add_executable(${PROJECT_NAME} ${HEADERS} test_lib.cpp)

target_link_libraries(${PROJECT_NAME} PUBLIC Catch2 pthread)
target_link_libraries(${PROJECT_NAME} PUBLIC inoty)
target_link_libraries(${PROJECT_NAME} PUBLIC stdc++fs)

target_compile_options(${PROJECT_NAME} PRIVATE -Wall -std=c++1z -g -O0)
