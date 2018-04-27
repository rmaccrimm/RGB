cmake_minimum_required(VERSION 3.9)
project(GB_Emulator)
add_subdirectory(src)
add_subdirectory(test)
enable_testing()
add_test(GB_Tests gb_tests)




