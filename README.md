# RGB

A Game Boy emulator written in C++ for Windows/Linux. My goal for this project is to learn about Game Boy hardware and emulation and get most Game Boy games playable.

![dr mario](/screenshots/dr_mario.jpg)![super mario land](/screenshots/super_mario_land.jpg)

## Features
- Video emulation using OpenGL to render graphics
- Sound emulation with SDL2
- Currently supports MBC1, MBC2 and MBC3 (minus real-time clock) cartridge types
- Includes a command line debugger for stepping through instructions, setting break points and viewing memory and register contents
- Keyboard controls (not currently changeable) - arrow keys for d-pad, A, B, enter and backspace for a, b, start and select

## Building
- Uses CMake to build
- Depends on OpenGL, SDL2, and Booost program_options

## Usage
- After compiling run from command line with the filename of the ROM to load as first argument. Use --help or -h to see all commands

## Tests

Tested using [Blargg's test ROMs](https://github.com/retrio/gb-test-roms), and the [Mooneye GB](https://github.com/Gekkio/mooneye-gb)'s test ROMs. Currently at least the following are passing:

![cpu_instrs.gb](/screenshots/blargg_cpu_instrs.jpg)![instr_timing.gb](/screenshots/blargg_instr_timing.jpg)

### Mooneye - acceptannce

- [ ] add_sp_e_timing.gb
- [ ] boot_hwio-dmg0.gb
- [ ] boot_hwio-dmgABCmgb.gb
- [ ] boot_hwio-S.gb
- [ ] boot_regs-dmg0.gb
- [x] boot_regs-dmgABC.gb
- [ ] boot_regs-mgb.gb
- [ ] boot_regs-sgb.gb
- [ ] boot_regs-sgb2.gb
- [ ] call_cc_timing.gb
- [ ] call_cc_timing2.gb
- [ ] call_timing.gb
- [ ] call_timing2.gb
- [ ] di_timing-GS.gb
- [ ] div_timing.gb
- [ ] ei_sequence.gb
- [x] halt_ime0_ei.gb
- [ ] halt_ime0_nointr_timing.gb
- [x] halt_ime1_timing.gb
- [ ] halt_ime1_timing2-GS.gb
- [x] if_ie_registers.gb
- [ ] intr_timing.gb
- [ ] jp_cc_timing.gb
- [ ] jp_timing.gb
- [ ] ld_hl_sp_e_timing.gb
- [ ] oam_dma_restart.gb
- [ ] pop_timing.gb
- [ ] push_timing.gb
- [ ] rapid_di_ei.gb
- [ ] ret_cc_timing.gb
- [ ] ret_timing.gb
- [x] reti_intr_timing.gb
- [ ] reti_timing.gb
- [x] bits/mem_oam.gb
- [x] bits/reg_f.gb
- [x] bits/unused_hwio-GS.gb
- [ ] interrupts/ie_push
- [x] oam_dma/basic.gb
- [ ] oam_dma/reg_read.gb
- [ ] oam_dma/sources-dmgABCmgbS.gb
- [ ] ppu/hblank_ly_scx_timing-GS.gb
- [ ] ppu/intr_1_2_timing-GS.gb
- [ ] ppu/intr_2_0_timing.gb
- [ ] ppu/intr_2_mode0_timing.gb
- [ ] ppu/intr_2_mode0_timing_sprites.gb
- [ ] ppu/intr_2_mode3_timing.gb
- [ ] ppu/intr_2_oam_ok_timing.gb
- [ ] ppu/lcdon_timing-dmgABCmgbS.gb
- [ ] ppu/lcdon_write_timing-GS.gb
- [ ] ppu/stat_irq_blocking.gb
- [ ] ppu/stat_lyc_onoff.gb
- [ ] ppu/vblank_stat_intr-GS.gb
- [ ] serial/boot_sclk_align-dmgABCmgb.gb
- [ ] timer/div_write.gb
- [ ] timer/rapid_toggle.gb
- [ ] timer/tim00.gb
- [x] timer/tim00_div_trigger.gb
- [x] timer/tim01.gb
- [ ] timer/tim01_div_trigger.ab
- [ ] timer/tim10.gb
- [ ] timer/tim10_div_trigger.gb
- [ ] timer/tim11.gb
- [x] timer/tim11_div_trigger.gb
- [ ] timer/tima_reload.gb
- [ ] timer/tima_write_reloading.gb
- [ ] timer/tma_write_reloading.gb
