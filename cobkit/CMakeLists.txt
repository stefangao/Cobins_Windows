cmake_minimum_required(VERSION 2.8)

project(cobkit)

add_compile_options(-std=c++11)

set(SOLUTION_ROOT_DIR ${PROJECT_SOURCE_DIR}/..)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

include_directories(${SOLUTION_ROOT_DIR}/inc)
include_directories(${SOLUTION_ROOT_DIR}/cobkit)
include_directories(${SOLUTION_ROOT_DIR}/external)

file(GLOB_RECURSE DIR_HEADERS  *.h)
source_group("Include" FILES ${DIR_HEADERS}) 

aux_source_directory(base DIR_LIB_SRCS)
aux_source_directory(winutils DIR_LIB_SRCS)
aux_source_directory(core DIR_LIB_SRCS)

add_library (${PROJECT_NAME} ${DIR_LIB_SRCS} ${DIR_HEADERS})
