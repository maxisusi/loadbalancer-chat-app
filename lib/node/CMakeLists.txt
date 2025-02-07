add_library(node src/node.cpp include/node.h)
target_link_libraries(node PUBLIC logger)
target_include_directories(node PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
