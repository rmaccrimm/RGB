include_directories(${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR})

find_package(SDL2 REQUIRED)
find_package(GLEW REQUIRED)
include_directories(${GLEW_INCLUDE_DIRS})
link_libraries(${GLEW_LIBRARIES})
find_package(OpenGL REQUIRED)

add_library(funcs functions.cpp)
add_library(proc processor.cpp register16bit.cpp register8bit.cpp)
add_library(mem mmu.cpp joypad.cpp)
add_library(ops operations.cpp)

add_executable(main 
    definitions.cpp
    main.cpp 
    debug.cpp
    window.cpp
    gpu.cpp
)
target_link_libraries(mem funcs)
target_link_libraries(ops funcs)
target_link_libraries(main funcs proc mem ops SDL2::SDL2 GLEW::GLEW ${OPENGL_gl_LIBRARY})
