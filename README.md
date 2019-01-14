# RGB

A Game Boy emulator written in C++ for Windows/Linux. My goal for this project is to learn about Game Boy hardware and emulation and get most Game Boy games playable.

![dr mario](/screenshots/dr_mario.jpg)![super mario land](/screenshots/super_mario_land.jpg)

# Features
- Video emulation using OpenGL to render graphics
- Sound emulation with SDL2
- Currently supports MBC1, MBC2 and MBC3 (minus real-time clock) cartridge types
- Includes a command line debugger for stepping through instructions, setting break points and viewing memory and register contents

# Building
- Uses CMake to build
- Depends on OpenGL, SDL2, and Booost program_options

# Usage
- After compiling run from command line with the filename of the ROM to load as first argument. Use --help or -h to see all commands

## Tests

Tested using [Blargg's test ROMs](https://github.com/retrio/gb-test-roms), and the [Mooneye GB](https://github.com/Gekkio/mooneye-gb)'s test ROMs. Currently at least the following are passing:

![cpu_instrs.gb](/screenshots/blargg_cpu_instrs.jpg)![instr_timing.gb](/screenshots/blargg_instr_timing.jpg)
