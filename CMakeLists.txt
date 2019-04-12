cmake_minimum_required(VERSION 2.8)

project(cobins_project)

add_compile_options(-std=c++11)

add_subdirectory(${PROJECT_SOURCE_DIR}/llfsm ${PROJECT_SOURCE_DIR}/llfsm/build)
add_subdirectory(${PROJECT_SOURCE_DIR}/winutils ${PROJECT_SOURCE_DIR}/winutils/build)
add_subdirectory(${PROJECT_SOURCE_DIR}/core ${PROJECT_SOURCE_DIR}/core/build)
add_subdirectory(${PROJECT_SOURCE_DIR}/spy_dll ${PROJECT_SOURCE_DIR}/spy_dll/build)
add_subdirectory(${PROJECT_SOURCE_DIR}/base ${PROJECT_SOURCE_DIR}/base/build)

