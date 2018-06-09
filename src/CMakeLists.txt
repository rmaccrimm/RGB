include_directories(${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR})

add_library(funcs functions.cpp)

find_package(SDL2 REQUIRED)
find_package(GLEW REQUIRED)
find_package(OpenGL REQUIRED)

add_executable(main 
    definitions.cpp
    main.cpp 
    debug.cpp
    window.cpp
    processor.cpp
    gpu.cpp
    register8bit.cpp
    register16bit.cpp
    operations.cpp
    memory.cpp
)
target_link_libraries(main funcs SDL2::SDL2 GLEW::GLEW ${OPENGL_gl_LIBRARY})