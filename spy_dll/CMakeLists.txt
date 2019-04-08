cmake_minimum_required(VERSION 2.8)

project(spy_dll)

add_compile_options(-std=c++11)

set(SOLUTION_ROOT_DIR ${PROJECT_SOURCE_DIR}/..)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

include_directories(${SOLUTION_ROOT_DIR}/inc)
include_directories(${SOLUTION_ROOT_DIR}/winutils)
include_directories(${SOLUTION_ROOT_DIR}/core)

link_directories(${SOLUTION_ROOT_DIR}/lib)

file(GLOB_RECURSE DIR_HEADERS  *.h)
source_group("Include" FILES ${DIR_HEADERS}) 

aux_source_directory(. DIR_LIB_SRCS)

add_library (${PROJECT_NAME} SHARED ${DIR_LIB_SRCS} ${DIR_HEADERS})
