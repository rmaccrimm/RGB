cmake_minimum_required(VERSION 3.9)
project(GB_Tests)
include_directories(include ../include)

set(SRCS 
    ../src/functions.cpp
    unittests/test_processor.cpp
)

add_executable(gb_tests ${SRCS})
target_link_libraries(gb_tests lib/gtestd)


