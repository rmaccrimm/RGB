#include "apu.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_Audio.h>

void APU::play()
{
    SDL_AudioSpec spec, obtained;
    SDL_AudioDeviceID device;

    spec.freq = 48000;
    spec.format = AUDIO_F32;
    spec.channels = 2;
    spec.samples = 4096;

    device = SDL_OpenAudioDevice(NULL, 0, &spec, &obtained, SDL_AUDIO_ALLOW_FORMAT_CHANGE);
    if (device == 0) {
        SDL_Log("Failed to open audio: %s", SDL_GetError());
    } else {
        if (obtained.format != spec.format) { /* we let this one thing change. */
            SDL_Log("We didn't get Float32 audio format.");
        }
        SDL_PauseAudioDevice(device, 0); 
        SDL_Delay(5000); 
        SDL_CloseAudioDevice(device);
    }
}