include_directories(${PROJECT_SOURCE_DIR}/include)
add_library(emu_srcs
    debug.cpp
    opcodes.cpp
    processor.cpp
    register8bit.cpp
    register16bit.cpp
    functions.cpp
    )
add_executable(gb_emulator main.cpp)
target_link_libraries(gb_emulator emu_srcs)