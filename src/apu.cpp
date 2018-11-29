#include "apu.h"
#include "registers.h"
#include <iostream>
#include <cmath>

int v = 0;
const Sint16 AMPLITUDE = 6000;
const int FREQUENCY1 = 300;
const int FREQUENCY2 = 60;
int per_s = 48000;
int per_call = 1024;
double call_freq = (double)per_s / (double)per_call;
double dt = 1.0f / call_freq/ (double)1024.0f;
double t = 0;

APU::APU(Memory *mem) : 
    memory{mem},
    channel_1{
        mem->get_mem_reference(reg::NR10),
        mem->get_mem_reference(reg::NR11),
        mem->get_mem_reference(reg::NR12),
        mem->get_mem_reference(reg::NR13),
        mem->get_mem_reference(reg::NR14)
    },
    channel_2{
        mem->get_mem_reference(reg::NR21),
        mem->get_mem_reference(reg::NR22),
        mem->get_mem_reference(reg::NR23),
        mem->get_mem_reference(reg::NR24)
    },
    channel_3{
        mem->get_mem_reference(reg::NR30),
        mem->get_mem_reference(reg::NR31),
        mem->get_mem_reference(reg::NR32),
        mem->get_mem_reference(reg::NR33),
        mem->get_mem_reference(reg::NR34)
    },
    channel_4{
        mem->get_mem_reference(reg::NR41),
        mem->get_mem_reference(reg::NR42),
        mem->get_mem_reference(reg::NR43),
        mem->get_mem_reference(reg::NR44)
    }
{
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
 }

APU::~APU() {
    SDL_CloseAudioDevice(device_id);
    SDL_Quit();
}

void APU::step(int cycles)
{

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
        _stream[2*i] = AMPLITUDE * std::sin(t * 2.0f * 3.14159f * FREQUENCY1);
        _stream[2*i + 1] = AMPLITUDE * std::sin(t * 2.0f * 3.14159f * FREQUENCY2);
        t += dt;
    }
}

int APU::sample_channel_1()
{

}