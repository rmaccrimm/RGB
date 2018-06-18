include_directories(include ${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR})
add_executable(gb_tests 
    unittests/test_main.cpp
    unittests/test_carries.cpp
    unittests/test_ops.cpp)
target_link_libraries(gb_tests funcs proc mem ops)


