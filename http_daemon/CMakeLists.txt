cmake_minimum_required(VERSION 3.7)

project(http_server)

find_package(PostgreSQL REQUIRED)

set(INC_PATH "include")
set(SRC_PATH "src")

file(GLOB HEADERS ${INC_PATH}/*.h)
file(GLOB SOURCES ${SRC_PATH}/*.cpp)

include_directories(${INC_PATH})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../ini_file_parser/include/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../pet_logger/include/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../TDB_workers/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../base_daemon/include)
include_directories(${PostgreSQL_INCLUDE_DIRS})

add_library(${PROJECT_NAME} STATIC ${SOURCES} ${HEADERS})

target_compile_options(${PROJECT_NAME} PRIVATE -Wall -std=c++1z -g -O0)
