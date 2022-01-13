cmake_minimum_required(VERSION 3.7)

project(TDB)

find_package(PostgreSQL REQUIRED)

set(INC_PATH "include")
set(SRC_PATH "src")
set(JSON1 "json1")

include_directories(${INC_PATH})
include_directories(${PostgreSQL_INCLUDE_DIRS})
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../ini_file_parser/include/")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../")

add_library(${JSON1} SHARED ${SRC_PATH}/json1.c ${INC_PATH}/sqlite3ext.h)
target_compile_options(${JSON1} PRIVATE -g -DSQLITE_ENABLE_JSON1)

add_library(${PROJECT_NAME}_postgre STATIC 

	${SRC_PATH}/TDB_postgre_worker.cpp 
	${INC_PATH}/TDB_postgre_worker.h
)
target_compile_options(${PROJECT_NAME}_postgre PRIVATE -g -O0)

add_library(${PROJECT_NAME}_SQLite STATIC 

	${SRC_PATH}/TDB_SQLite_worker.cpp
	${SRC_PATH}/sqlite3.c
	${INC_PATH}/sqlite3.h
	${INC_PATH}/TDB_SQLite_worker.h
)
target_compile_options(${PROJECT_NAME}_SQLite PRIVATE -g -O0)

add_library(${PROJECT_NAME}_fabrique STATIC 

	${SRC_PATH}/TDB_workers_fabrique.cpp
	${INC_PATH}/TDB_workers_fabrique.h
)
target_compile_options(${PROJECT_NAME}_fabrique PRIVATE -g -O0)

install(TARGETS ${JSON1} DESTINATION /usr/lib)
