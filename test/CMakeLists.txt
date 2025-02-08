project(hello_test)
add_executable(hello_test test.cpp)

target_link_libraries(hello_test PRIVATE gtest protocat)

add_test(HelloTest hello_test)
