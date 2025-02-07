project(logger)
add_library(logger "src/logger.cpp" "include/logger.hpp")

target_include_directories(logger PUBLIC # Use absolute path to avoid ambiguty
                                         "${CMAKE_CURRENT_SOURCE_DIR}/include")
