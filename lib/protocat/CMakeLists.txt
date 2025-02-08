add_library(protocat src/protocat.cpp include/protocat.hpp)
target_include_directories(protocat PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
