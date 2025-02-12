add_library(sql_driver src/sqldriver.cpp include/sqldriver.hpp)
target_link_libraries(sql_driver logger)
target_include_directories(sql_driver
                           PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
