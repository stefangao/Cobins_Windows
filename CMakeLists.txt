cmake_minimum_required(VERSION 2.8)
project(mmi_cobins)

add_compile_options(-std=c++11)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

add_subdirectory(${PROJECT_SOURCE_DIR}/winutils)
add_subdirectory(${PROJECT_SOURCE_DIR}/core)
add_subdirectory(${PROJECT_SOURCE_DIR}/llfsm)

#共享DLL中使用 
ADD_DEFINITIONS(-D_AFXDLL)
SET(CMAKE_MFC_FLAG 2)
add_definitions(-DUNICODE -D_UNICODE)

include_directories(
${PROJECT_SOURCE_DIR}/inc
${PROJECT_SOURCE_DIR}/core
${PROJECT_SOURCE_DIR}/llfsm
${PROJECT_SOURCE_DIR}/winutils
)

link_directories(
${PROJECT_SOURCE_DIR}/lib
${PROJECT_SOURCE_DIR}/core/lib
${PROJECT_SOURCE_DIR}/llfsm/lib
${PROJECT_SOURCE_DIR}/core/lib
${PROJECT_SOURCE_DIR}/winutils/lib
)


aux_source_directory(mmi_exe DIR_SRCS)

add_executable(mmi_cobins WIN32 ${DIR_SRCS})

target_link_libraries(mmi_cobins llfsm)
target_link_libraries(mmi_cobins core)
target_link_libraries(mmi_cobins winutils)
