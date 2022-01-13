cmake_minimum_required(VERSION 3.7)

project(ini_file_parser)

find_package(Boost REQUIRED)

set(INC_PATH "include")
set(SRC_PATH "src")

file(GLOB HEADERS ${INC_PATH}/*.h)
file(GLOB SOURCES ${SRC_PATH}/*.cpp)

include_directories(${INC_PATH})

add_library(${PROJECT_NAME} STATIC ${SOURCES} ${HEADERS})

target_link_libraries(${PROJECT_NAME} PUBLIC boost)

target_compile_options(${PROJECT_NAME} PRIVATE -Wall -std=c++1z -g -O0)