cmake_minimum_required(VERSION 2.8)

project(spy_dll)

add_compile_options(-std=c++11)

set(SOLUTION_ROOT_DIR ${PROJECT_SOURCE_DIR}/..)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

include_directories(${SOLUTION_ROOT_DIR}/inc)
include_directories(${SOLUTION_ROOT_DIR}/external)
include_directories(${SOLUTION_ROOT_DIR}/cobkit)
include_directories(${SOLUTION_ROOT_DIR}/cobkit/inc)

link_directories(${SOLUTION_ROOT_DIR}/lib)

file(GLOB_RECURSE DIR_HEADERS  *.h)
source_group("Include" FILES ${DIR_HEADERS}) 

aux_source_directory(. DIR_LIB_SRCS)

add_library(${PROJECT_NAME} SHARED ${DIR_LIB_SRCS} ${DIR_HEADERS})

target_link_libraries(${PROJECT_NAME} llfsm)
target_link_libraries(${PROJECT_NAME} cobkit)

add_custom_command(
    TARGET ${PROJECT_NAME}
    COMMAND ${CMAKE_COMMAND} -E copy ${LIBRARY_OUTPUT_PATH}/Debug/${PROJECT_NAME}.dll ${SOLUTION_ROOT_DIR}/bin
)
