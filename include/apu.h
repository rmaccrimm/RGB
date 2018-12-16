#ifndef APU_H
#define APU_H

#include "definitions.h"
#include "mmu.h"
#include <vector>
#include <SDL2/SDL.h>
#include <SDL2/SDL_audio.h>
#include <map>
#include <vector>
#include <iterator>

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

    void read_registers();

    void clock_length_counters();

    void clock_freq_sweep();

    void clock_vol_envelope();

    const Memory* memory;

    SDL_AudioDeviceID device_id;

    unsigned int clock;

    unsigned int frame_step;

    struct {
        bool enable;
        int counter;
        bool decrement_counter;
        bool restart;
        int frequency;
        int duty;
        int sweep_time;
        bool sweep_direction;
        int sweep_shift;
        int initial_volume;
        bool increase_volume;
        int volume_sweep;
        int volume;
        int volume_clock;
    } channel_1;

    struct {
        bool enable;
        int counter;
        bool decrement_counter;
        bool restart;
        int frequency;
        int duty;
        int initial_volume;
        bool increase_volume;
        int volume_sweep;
        int volume;
        int volume_clock;
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

    u8 &reg_nr10;
    u8 &reg_nr11;
    u8 &reg_nr12;
    u8 &reg_nr13;
    u8 &reg_nr14;
    u8 &reg_nr21;
    u8 &reg_nr22;
    u8 &reg_nr23;
    u8 &reg_nr24;
    u8 &reg_nr30;
    u8 &reg_nr31;
    u8 &reg_nr32;
    u8 &reg_nr33;
    u8 &reg_nr34;
    u8 &reg_nr41;
    u8 &reg_nr42;
    u8 &reg_nr43;
    u8 &reg_nr44;
    u8 &reg_nr50;
    u8 &reg_nr51;
    u8 &reg_nr52;
    
    std::map<u16, u8*> audio_reg;
};

#endif