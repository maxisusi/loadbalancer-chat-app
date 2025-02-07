cmake_minimum_required(VERSION 3.10)
project(chatqui)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(chatqui src/main.cpp)
add_subdirectory(lib)

target_link_libraries(chatqui PRIVATE logger node sql_driver)

target_include_directories(chatqui PUBLIC "${PROJECT_BINARY_DIR}")
