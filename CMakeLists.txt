cmake_minimum_required(VERSION 3.9)
project(GB_Emulator)
add_subdirectory(src)

cmake_policy(SET CMP0072 NEW)

# if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    SET(GCC_DEBUG_COMPILE_FLAGS "-fsanitize=address -Og -pg -ggdb -Wall -Wextra -Wshadow \
        -Wduplicated-cond -Wfatal-errors -Wlogical-op -Wnull-dereference -Wuseless-cast")
    SET(GCC_DEBUG_LINK_FLAGS "-fsanitize=address")

    SET(GCC_RELEASE_COMPILE_FLAGS "-O3")

    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_CC_FLAGS_DEBUG} ${GCC_DEBUG_LINK_FLAGS}")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${GCC_DEBUG_COMPILE_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} ${GCC_DEBUG_LINK_FLAGS}")

    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${GCC_DEBUG_COMPILE_FLAGS}")
    
# endif()
