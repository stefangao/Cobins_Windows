cmake_minimum_required(VERSION 2.8)
project(core)

add_compile_options(-std=c++11)

set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

include_directories(${PROJECT_SOURCE_DIR}/../inc)
include_directories(${PROJECT_SOURCE_DIR}/../llfsm)

aux_source_directory(. DIR_LIB_SRCS)

add_library (winutils ${DIR_LIB_SRCS})


