#include "apu.h"
#include "registers.h"
#include "util.h"
#include <iostream>

#define _USE_MATH_DEFINES
#include <cmath>

int v = 0;
const Sint16 AMPLITUDE = 600;
const int FREQUENCY1 = 300;
const int FREQUENCY2 = 60;
int per_s = 48000;
int per_call = 1024;
double call_freq = (double)per_s / (double)per_call;
double dt = 1.0f / call_freq/ (double)1024.0f;
double t = 0;

APU::APU(Memory *mem) : memory{mem}
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
    update_registers();
 }

APU::~APU() {
    SDL_CloseAudioDevice(device_id);
    SDL_Quit();
}

void APU::step(int cycles)
{
    clock += cycles;
    // Audio control registers update at 256 Hz, which means 1 tick per 2^14 cpu cycles 

    if (clock >= 0x4000) {
        clock -= 0x4000; 
    }
}

int APU::square_wave(double t, double freq, int amp, int duty)
{
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
    if (!utils::bit(sound_control.on_off, 7)) {
        // return;
    }
    Sint16* _stream = (Sint16*)stream;
    for (int i = 0; i < len/4; i++) {
        _stream[2*i] = 0;
        _stream[2*i + 1] = 0;
        // if (utils::bit(sound_control.mix, 1)) {
            _stream[2*i] = sample_channel_2();
        // }
        if (utils::bit(sound_control.mix, 5)) {
            _stream[2*i + 1] = sample_channel_2();
        }
        t += dt;
    }
}

int APU::sample_channel_1()
{

}

int APU::sample_channel_2()
{

}

void APU::update_registers()
{
    
}

void APU::init_reg_references()
{
    u16 io_base_addr = 0xff00;
    audio_reg[reg::NR10] = memory->io_registers[reg::NR10 - io_base_addr];
    audio_reg[reg::NR11] = memory->io_registers[reg::NR11 - io_base_addr];
    audio_reg[reg::NR12] = memory->io_registers[reg::NR12 - io_base_addr];
    audio_reg[reg::NR13] = memory->io_registers[reg::NR13 - io_base_addr];
    audio_reg[reg::NR14] = memory->io_registers[reg::NR14 - io_base_addr];
    audio_reg[reg::NR21] = memory->io_registers[reg::NR21 - io_base_addr];
    audio_reg[reg::NR22] = memory->io_registers[reg::NR22 - io_base_addr];
    audio_reg[reg::NR23] = memory->io_registers[reg::NR23 - io_base_addr];
    audio_reg[reg::NR24] = memory->io_registers[reg::NR24 - io_base_addr];
    audio_reg[reg::NR30] = memory->io_registers[reg::NR30 - io_base_addr];
    audio_reg[reg::NR31] = memory->io_registers[reg::NR31 - io_base_addr];
    audio_reg[reg::NR32] = memory->io_registers[reg::NR32 - io_base_addr];
    audio_reg[reg::NR33] = memory->io_registers[reg::NR33 - io_base_addr];
    audio_reg[reg::NR34] = memory->io_registers[reg::NR34 - io_base_addr];
    audio_reg[reg::NR41] = memory->io_registers[reg::NR41 - io_base_addr];
    audio_reg[reg::NR42] = memory->io_registers[reg::NR42 - io_base_addr];
    audio_reg[reg::NR43] = memory->io_registers[reg::NR43 - io_base_addr];
    audio_reg[reg::NR44] = memory->io_registers[reg::NR44 - io_base_addr];
    audio_reg[reg::NR50] = memory->io_registers[reg::NR50 - io_base_addr];
    audio_reg[reg::NR51] = memory->io_registers[reg::NR51 - io_base_addr];
    audio_reg[reg::NR52] = memory->io_registers[reg::NR52 - io_base_addr];
}