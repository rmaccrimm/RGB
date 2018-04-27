include_directories(include)
set(SRCS 
    src/debug.cpp
    src/opcodes.cpp
    src/processor.cpp
    src/register8bit.cpp
    src/register16bit.cpp
    src/functions.cpp)
add_executable(gb_emulator ${SRCS})
