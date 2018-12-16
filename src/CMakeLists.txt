if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    SET(GCC_DEBUG_COMPILE_FLAGS "-fsanitize=address -Og -pg -ggdb -Wall -Wextra -Wshadow \
        -Wduplicated-cond -Wfatal-errors -Wlogical-op -Wnull-dereference -Wuseless-cast")
    SET(GCC_DEBUG_LINK_FLAGS "-fsanitize=address")

    SET(GCC_RELEASE_COMPILE_FLAGS "-O3")

    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_CC_FLAGS_DEBUG} ${GCC_DEBUG_COMPILE_FLAGS}")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${GCC_DEBUG_COMPILE_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} ${GCC_DEBUG_LINK_FLAGS}")

    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${GCC_RELEASE_COMPILE_FLAGS}")
    
endif()

configure_file(${PROJECT_SOURCE_DIR}/include/config.h.in ${CMAKE_BINARY_DIR}/generated/config.h)

include_directories(
    ${PROJECT_SOURCE_DIR}/include 
    ${PROJECT_SOURCE_DIR} 
    ${CMAKE_BINARY_DIR}/generated)

find_package(SDL2 REQUIRED)
find_package(GLEW REQUIRED)
find_package(OpenGL REQUIRED)
find_package(Boost COMPONENTS program_options REQUIRED)
include_directories(${GLEW_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS})
link_libraries(${GLEW_LIBRARIES})

add_library(funcs util.cpp)
add_library(proc processor.cpp)
add_library(mem mmu.cpp joypad.cpp cartridge.cpp)
add_library(ops operations.cpp)
add_library(apu apu.cpp)

add_executable(main 
    definitions.cpp
    main.cpp 
    debug.cpp
    window.cpp
    gpu.cpp
)
target_link_libraries(mem funcs)
target_link_libraries(ops funcs)
target_link_libraries(main funcs proc mem ops apu SDL2::SDL2 GLEW::GLEW ${OPENGL_gl_LIBRARY} ${Boost_LIBRARIES})
