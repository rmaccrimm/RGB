#ifndef APU_H
#define APU_H

#include <vector>
#include <SDL2/SDL.h>
#include <SDL2/SDL_audio.h>
#include "definitions.h"
#include "mmu.h"

class APU
{
public:
    APU(Memory *mem);
    ~APU();

    void step(int cycles);

    void start();

// private:
    const Memory* memory;

    SDL_AudioDeviceID device_id;

    struct {
        u16 sweep;
        u16 sound_length_and_duty_cycle;
        u16 volume_envelope;
        u16 frequency_low;
        u16 frequency_high;
    } channel_1;

    struct {
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
    } channel_4;

    // SDL audio callback function. Forwards call to APU object pointed ot by userdata
    static void forward_callback(void *APU_obj, Uint8 *stream, int len);

    // Actual function to fill audio buffer
    void audio_callback(Uint8 *stream, int len);

    int square_wave(double t, double freq, int amplitude, int duty);

    int sample_channel_1();

    int sample_channel_2();

    int sample_channel_3();

    int sample_channel_4();
};



#endif