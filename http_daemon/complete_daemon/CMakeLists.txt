cmake_minimum_required(VERSION 3.7)

project(http_daemon)

find_package(Boost REQUIRED)
find_package(PostgreSQL REQUIRED)

set(INC_PATH "../include")
set(LIB_PATH "../build/")

file(GLOB HEADERS ${INC_PATH}/*.h)

include_directories(${INC_PATH})
include_directories(${PostgreSQL_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../ini_file_parser/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../pet_logger/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../TDB_workers/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../base_daemon/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../)

link_directories(${PROJECT_NAME} ${LIB_PATH})
link_directories(${PROJECT_NAME} ../../ini_file_parser/build/)
link_directories(${PROJECT_NAME} ../../pet_logger/build/)
link_directories(${PROJECT_NAME} ../../TDB_workers/build/)
link_directories(${PROJECT_NAME} ../../base_daemon/build/)

add_executable(${PROJECT_NAME} ${HEADERS} http_daemon.cpp)

target_link_libraries(${PROJECT_NAME} PUBLIC 
	http_server	
	base_daemon
	ini_file_parser
	pet_logger
	TDB_fabrique
	TDB_SQLite
	TDB_postgre
	stdc++fs
	pthread
	event	
	pq
	dl
)

target_compile_options(${PROJECT_NAME} PRIVATE -Wall -std=c++1z -g -O0)
