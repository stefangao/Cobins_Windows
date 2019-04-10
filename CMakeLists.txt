cmake_minimum_required(VERSION 2.8)

project(cobins_project)

add_compile_options(-std=c++11)

add_subdirectory(${PROJECT_SOURCE_DIR}/llfsm)
add_subdirectory(${PROJECT_SOURCE_DIR}/winutils)
add_subdirectory(${PROJECT_SOURCE_DIR}/core)
add_subdirectory(${PROJECT_SOURCE_DIR}/mmi_exe)
add_subdirectory(${PROJECT_SOURCE_DIR}/spy_dll)

