#include "apu.h"
#include "registers.h"
#include <iostream>

#define _USE_MATH_DEFINES
#include <cmath>

int v = 0;
const Sint16 AMPLITUDE = 6000;
const int FREQUENCY1 = 300;
const int FREQUENCY2 = 70;
int per_s = 48000;
int per_call = 1024;
double call_freq = (double)per_s / (double)per_call;
double dt = 1.0f / call_freq/ (double)1024.0f;
double t = 0;

int square_wave(double t, double freq, int amp, int duty)
{
    double T = 1.0 / (2.0 * freq);
    return (static_cast<int>(t / T) % duty == 0 ? amp : -amp);
}

APU::APU(Memory *mem) : 
    memory{mem},
    channel_1{
        mem->get_mem_reference(reg::NR10),
        mem->get_mem_reference(reg::NR11),
        mem->get_mem_reference(reg::NR12),
        mem->get_mem_reference(reg::NR13),
        mem->get_mem_reference(reg::NR14)
    }
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
    spec.callback = audio_callback;
    spec.userdata = NULL;

    if (SDL_InitSubSystem(SDL_INIT_AUDIO) != 0) {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
    }
    audio_device = SDL_OpenAudioDevice(NULL, 0, &spec, &obtained, 0);
    if (audio_device == 0) {
        SDL_Log("Failed to open audio: %s", SDL_GetError());
    }
 }

APU::~APU() {
    SDL_CloseAudioDevice(audio_device);
    SDL_Quit();
}

void audio_callback(void *userdata, Uint8 *stream, int len)
{
    Sint16* _stream = (Sint16*)stream;
    for (int i = 0; i < len/4; i++) {
        // _stream[2*i] = AMPLITUDE * std::sin(t * 2.0f * 3.14159f * FREQUENCY1);
        // _stream[2*i + 1] = AMPLITUDE * std::sin(t * 2.0f * 3.14159f * FREQUENCY2);
        // _stream[2*i] = std::sin(t * 2.0f * M_PI * FREQUENCY1) >= 0 ? AMPLITUDE : -AMPLITUDE;
        // _stream[2*i + 1] = std::sin(t * 2.0f * M_PI * FREQUENCY2) >= 0 ? AMPLITUDE : -AMPLITUDE;
        _stream[2*i] = square_wave(t, FREQUENCY1, AMPLITUDE, 2); 
        _stream[2*i + 1] = square_wave(t, FREQUENCY2, AMPLITUDE, 2); 
        t += dt;
    }
}

int APU::channel_1_sample()
{
    double sweep_time = ((channel_1.sweep >> 4) & 7) / 128.0;
    if (sweep_time != 0) {
        double df = 1.0 / std::pow(2, (channel_1.sweep & 3));
    }
}

void APU::play()
{
    SDL_PauseAudioDevice(audio_device, 0); 
}