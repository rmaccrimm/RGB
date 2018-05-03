include_directories(${PROJECT_SOURCE_DIR}/include)

add_library(processor
    debug.cpp
    opcodes.cpp
    processor.cpp
    register8bit.cpp
    register16bit.cpp
    functions.cpp)

find_package(SDL2 REQUIRED)
find_package(GLEW REQUIRED)
find_package(OpenGL REQUIRED)

add_executable(main main.cpp)
target_link_libraries(main processor SDL2::SDL2 GLEW::GLEW ${OPENGL_gl_LIBRARY})