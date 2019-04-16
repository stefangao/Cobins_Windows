cmake_minimum_required(VERSION 2.8)

project(mmi_exe)

add_compile_options(-std=c++11)

set(SOLUTION_ROOT_DIR ${PROJECT_SOURCE_DIR}/..)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

ADD_DEFINITIONS(-D_AFXDLL)
SET(CMAKE_MFC_FLAG 2)
add_definitions(-DUNICODE -D_UNICODE)

include_directories(
${SOLUTION_ROOT_DIR}/inc
${SOLUTION_ROOT_DIR}/external
${SOLUTION_ROOT_DIR}/cobins
${SOLUTION_ROOT_DIR}/cobins/inc
${SOLUTION_ROOT_DIR}/llfsm
)

link_directories(
${SOLUTION_ROOT_DIR}/lib
${SOLUTION_ROOT_DIR}/base/lib
${SOLUTION_ROOT_DIR}/llfsm/lib
${SOLUTION_ROOT_DIR}/core/lib
${SOLUTION_ROOT_DIR}/winutils/lib
)

file(GLOB_RECURSE DIR_HEADERS  *.h)
source_group("Include" FILES ${DIR_HEADERS}) 

aux_source_directory(. DIR_SRCS)

add_executable(${PROJECT_NAME} WIN32 ${DIR_SRCS} ${DIR_HEADERS})

target_link_libraries(${PROJECT_NAME} llfsm)
target_link_libraries(${PROJECT_NAME} base)
target_link_libraries(${PROJECT_NAME} core)
target_link_libraries(${PROJECT_NAME} winutils)

add_custom_command(
    TARGET ${PROJECT_NAME}
    COMMAND ${CMAKE_COMMAND} -E copy ${EXECUTABLE_OUTPUT_PATH}/Debug/${PROJECT_NAME}.exe ${SOLUTION_ROOT_DIR}/bin
)

