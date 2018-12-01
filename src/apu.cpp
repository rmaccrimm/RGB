#include "apu.h"
#include "registers.h"
#include "util.h"
#include <iostream>

#define _USE_MATH_DEFINES
#include <cmath>

int v = 0;
const Sint16 AMPLITUDE = 50;
const int FREQUENCY1 = 300;
const int FREQUENCY2 = 60;
int per_s = 48000;
int per_call = 1024;
double call_freq = (double)per_s / (double)per_call;
double dt = 1.0f / call_freq/ (double)1024.0f;
double t = 0;

APU::APU(Memory *mem) : 
    memory{mem},
    clock{0},
    frame_step{0},
    reg_nr10{mem->get_mem_reference(reg::NR10)},
    reg_nr11{mem->get_mem_reference(reg::NR11)},
    reg_nr12{mem->get_mem_reference(reg::NR12)},
    reg_nr13{mem->get_mem_reference(reg::NR13)},
    reg_nr14{mem->get_mem_reference(reg::NR14)},
    reg_nr21{mem->get_mem_reference(reg::NR21)},
    reg_nr22{mem->get_mem_reference(reg::NR22)},
    reg_nr23{mem->get_mem_reference(reg::NR23)},
    reg_nr24{mem->get_mem_reference(reg::NR24)},
    reg_nr30{mem->get_mem_reference(reg::NR30)},
    reg_nr31{mem->get_mem_reference(reg::NR31)},
    reg_nr32{mem->get_mem_reference(reg::NR32)},
    reg_nr33{mem->get_mem_reference(reg::NR33)},
    reg_nr34{mem->get_mem_reference(reg::NR34)},
    reg_nr41{mem->get_mem_reference(reg::NR41)},
    reg_nr42{mem->get_mem_reference(reg::NR42)},
    reg_nr43{mem->get_mem_reference(reg::NR43)},
    reg_nr44{mem->get_mem_reference(reg::NR44)},
    reg_nr50{mem->get_mem_reference(reg::NR50)},
    reg_nr51{mem->get_mem_reference(reg::NR51)},
    reg_nr52{mem->get_mem_reference(reg::NR52)}
{
    for (int i = 0; i < SDL_GetNumAudioDevices(0); i++) {
        SDL_Log("%s", SDL_GetAudioDeviceName(i, 0));
    }

    SDL_AudioSpec spec, obtained;
    SDL_zero(spec);

    spec.freq = per_s;
    spec.format = AUDIO_S16SYS;
    spec.channels = 2;
    spec.samples = per_call;
    spec.callback = APU::forward_callback;
    spec.userdata = this;

    if (SDL_InitSubSystem(SDL_INIT_AUDIO) != 0) {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
    }
    device_id = SDL_OpenAudioDevice(NULL, 0, &spec, &obtained, 0);
    if (device_id == 0) {
        SDL_Log("Failed to open audio: %s", SDL_GetError());
    }

    init_reg_references();
    read_registers();  

    channel_2.volume_clock = 0;
    channel_2.volume = channel_2.initial_volume;
}

APU::~APU() {
    SDL_CloseAudioDevice(device_id);
    SDL_Quit();
}

void APU::step(int cycles)
{
    clock += cycles;
    read_registers();

    if (channel_2.restart) {
        channel_2.volume = channel_2.initial_volume;
    }

    // Frame sequencer updates at 2^9 Hz, which means 1 tick per 2^13 cpu cycles 
    if (clock >= 0x2000) {
        clock -= 0x2000;

        frame_step++;
        frame_step %= 8;

        if ((frame_step & 1) == 0) {
            clock_length_counters();
        }
        if ((frame_step == 2 || frame_step == 6)) {
            // clock_freq_sweep();
        }
        if (frame_step == 7) {
            // clock_vol_envelope();
        }

    }
}

void APU::clock_vol_envelope()
{
    channel_2.volume_clock++;
    if (channel_2.volume_clock % channel_2.volume_sweep == 0) {
        if (channel_2.increase_volume) {
            channel_2.volume = std::min(15, channel_2.volume - 1);
        }
        else {
            channel_2.volume = std::max(0, channel_2.volume - 1);
        }
    }
}

void APU::clock_length_counters()
{
    if (channel_2.decrement_counter && channel_2.counter > 0) {
        reg_nr24 = (reg_nr24 && (7 << 5)) && ((reg_nr24 - 1) & 0x1f);
        channel_2.counter = reg_nr24;
    }
    reg_nr52 = utils::set_cond(reg_nr52, 1, channel_2.enable);
    channel_2.enable = channel_2.counter != 0;
}

int APU::square_wave(double t, double freq, int amp, int duty)
{
    freq = 131072.0 / freq;
    double D = duty == 0 ? 0.5 : duty;
    double T = 1.0 / (4.0 * freq);
    t -= 4 * T * std::floor(t / (4 * T));
    return t <= (D * T) ? amp : -amp;
}

void APU::start()
{
    SDL_PauseAudioDevice(device_id, 0); 
}

void APU::forward_callback(void *userdata, Uint8 *stream, int len)
{
    static_cast<APU*>(userdata)->audio_callback(stream, len);
}

void APU::audio_callback(Uint8 *stream, int len)
{
    Sint16* _stream = (Sint16*)stream;
    for (int i = 0; i < len/4; i++) {
        _stream[2*i] = sample_channel_2();
        _stream[2*i + 1] = sample_channel_2();
        t += dt;
    }
}

int APU::sample_channel_1()
{

}

int APU::sample_channel_2()
{
    if (channel_2.enable && sound_control.master_enable) {
        return square_wave(t, channel_2.frequency, channel_2.volume * AMPLITUDE, channel_2.duty);
    }
    else {
        return 0;
    }
}

void APU::read_registers()
{
    sound_control.master_enable = utils::bit(reg_nr52, 7);

    u8 freq_lo = reg_nr23;
    u8 freq_hi = reg_nr24 & 7;
    int x = (freq_hi << 8) | freq_lo & 0x7ff;
    channel_2.frequency = 0x800 - x;//(32 * (2048.0 - (double)x));
    // std::cout << channel_2.frequency << std::endl;
    channel_2.decrement_counter = utils::bit(reg_nr24, 6);
    channel_2.duty = (reg_nr21 >> 6) & 3;
    channel_2.initial_volume = (reg_nr22 >> 4 ) & 0xf;
    channel_2.increase_volume = utils::bit(reg_nr22, 3);
    channel_2.volume_sweep = reg_nr22 & 7;
    channel_2.restart = utils::bit(reg_nr24, 7);
}

void APU::init_reg_references()
{
    /*u16 io_base_addr = 0xff00;
    audio_reg[reg::NR10] = memory->get_mem_reference(reg::NR10);
    audio_reg[reg::NR11] = &memory->io_registers[reg::NR11 - io_base_addr];
    audio_reg[reg::NR12] = &memory->io_registers[reg::NR12 - io_base_addr];
    audio_reg[reg::NR13] = &memory->io_registers[reg::NR13 - io_base_addr];
    audio_reg[reg::NR14] = &memory->io_registers[reg::NR14 - io_base_addr];
    audio_reg[reg::NR21] = &memory->io_registers[reg::NR21 - io_base_addr];
    audio_reg[reg::NR22] = &memory->io_registers[reg::NR22 - io_base_addr];
    audio_reg[reg::NR23] = memory->io_registers[reg::NR23 - io_base_addr];
    audio_reg[reg::NR24] = memory->io_registers[reg::NR24 - io_base_addr];
    audio_reg[reg::NR30] = memory->io_registers[reg::NR30 - io_base_addr];
    audio_reg[reg::NR31] = memory->io_registers[reg::NR31 - io_base_addr];
    auxdio_reg[reg::NR32] = memory->io_registers[reg::NR32 - io_base_addr];
    audio_reg[reg::NR33] = memory->io_registers[reg::NR33 - io_base_addr];
    audio_reg[reg::NR34] = memory->io_registers[reg::NR34 - io_base_addr];
    audio_reg[reg::NR41] = memory->io_registers[reg::NR41 - io_base_addr];
    audio_reg[reg::NR42] = memory->io_registers[reg::NR42 - io_base_addr];
    audio_reg[reg::NR43] = memory->io_registers[reg::NR43 - io_base_addr];
    audio_reg[reg::NR44] = memory->io_registers[reg::NR44 - io_base_addr];
    audio_reg[reg::NR50] = memory->io_registers[reg::NR50 - io_base_addr];
    audio_reg[reg::NR51] = memory->io_registers[reg::NR51 - io_base_addr];
    audio_reg[reg::NR52] = memory->io_registers[reg::NR52 - io_base_addr];*/
}