include_directories(include ${PROJECT_SOURCE_DIR}/include)
add_executable(gb_tests 
    unittests/test_main.cpp
    unittests/test_carries.cpp)
target_link_libraries(gb_tests emu_srcs)


