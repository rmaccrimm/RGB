#ifndef APU_H
#define APU_H

#include "definitions.h"
#include "mmu.h"
#include <vector>
#include <SDL2/SDL.h>
#include <SDL2/SDL_audio.h>
#include <map>

class APU
{
public:

    APU(Memory *mem);
    ~APU();

    void step(int cycles);

    void start();

private:
    // SDL audio callback function. Forwards call to APU object pointed ot by userdata
    static void forward_callback(void *APU_obj, Uint8 *stream, int len);

    // Actual function to fill audio buffer
    void audio_callback(Uint8 *stream, int len);

    int square_wave(double t, double freq, int amplitude, int duty);

    int sample_channel_1();

    int sample_channel_2();

    int sample_channel_3();

    int sample_channel_4();

    void init_reg_references();

    void update_registers();

    const Memory* memory;

    SDL_AudioDeviceID device_id;

    int clock;

    struct {
        int counter;
        bool stop;
        bool restart;
        int frequency;
        int duty;
        int sweep_time;
        bool sweep_direction;
        int sweep_shift;
        int envelope_init;
        bool envelope_direction;
        int envelope_sweep;
    } channel_1;

    struct {
        int counter;
        bool stop;
        bool restart;
        int frequency;
        int duty;
        int envelope_init;
        bool envelope_direction;
        int envelope_sweep;
    } channel_2;

    struct {
        bool enable;
        int counter;
        bool stop;
        int frequency;
        int volume;
    } channel_3;

    struct {
        int counter;
        int envelope_init;
        bool envelope_direction;
        int envelope_sweep;
        int shift_frequency;
        int step_width;
        int dividing_ratio;
    } channel_4;

    struct {
        bool master_enable;
        bool enable_left;
        bool enable_right;
        int volume_left;
        int volume_right;
        bool enable_channel_1;
        bool enable_channel_2;
        bool enable_channel_3;
        bool enable_channel_4;
        bool channel_1_left;
        bool channel_1_right;
        bool channel_2_left;
        bool channel_2_right;
        bool channel_3_left;
        bool channel_3_right;
        bool channel_4_left;
        bool channel_4_right;
    } sound_control;

    std::map<u16, u8&> audio_reg;
};



#endif