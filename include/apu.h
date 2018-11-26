#ifndef APU_H
#define APU_H

#include <vector>
#include <SDL2/SDL.h>
#include <SDL2/SDL_Audio.h>
#include "definitions.h"
#include "mmu.h"

void audio_callback(void *data, u8 *stream, int len);

class APU
{
public:
    APU(Memory *mem);
    ~APU();

    void play();

private:
    const Memory* memory;

    struct {
        u16 sweep;
        u16 sound_length_and_duty_cycle;
        u16 volume_envelope;
        u16 frequency_low;
        u16 frequency_high;
    } channel_1;

    /*struct {
        u16 sound_length_and_duty_cycle;
        u16 volume_envelope;
        u16 frequency_low;
        u16 frequency_high;
    } channel_2;

    struct {
        u16 enable;
        u16 sound_length;
        u16 output_level;
        u16 frequency_low;
        u16 frequency_high;
    } channel_3;

    struct {
        u16 sound_length;
        u16 volume_envelope;
        u16 polynomial_counter;
        u16 counter;
    } channel_4;*/


    SDL_AudioDeviceID audio_device;
};



#endif