#ifndef APU_H
#define APU_H

#include "definitions.h"
#include <vector>
#include <SDL2/SDL.h>
#include <SDL2/SDL_audio.h>
#include <map>
#include <vector>
#include <iterator>

class APU
{
public:

    APU();
    ~APU();

    void step(int cycles);

    void start();

    u8 read(u16 addr);
    
    void write(u16 addr, u8 data);

private:

    struct Channel
    {
        // Common to all channels
        bool playing;
        bool output_left;
        bool output_right;
        int length_counter;
        int sound_length;
        bool decrement_counter;
        bool trigger;
        int duty;
        int frequency;
        // Used by channels 1 and 2
        int initial_volume;
        bool increase_volume;
        int volume_sweep_time;
        int volume;
        int volume_clock;
        // Used by channel 1
        int initial_freq;
        bool increase_freq;
        int freq_sweep_enable;
        int freq_sweep_time;
        int freq_shift;
        int freq_clock;
        // channel 3
        bool enable;
    } channels[4];

    bool master_enable;
    bool enable_left;
    bool enable_right;
    int volume_left;
    int volume_right;

    std::map<u16, u8> registers;
    std::map<u16, u8> read_masks;
    std::vector<u8> wave_pattern_RAM;
    std::map<u16, bool> unused_addr;

    unsigned int clock;
    unsigned int frame_step;

    SDL_AudioDeviceID device_id;

    void reset();

    // SDL audio callback function. Forwards call to APU object pointed ot by userdata
    static void forward_callback(void *APU_obj, Uint8 *stream, int len);
    // Actual function to fill audio buffer
    void audio_callback(Uint8 *stream, int len);
    int square_wave(double t, double freq, int amplitude, int duty);

    int sample_channel(int channel_num);

    void clock_length_counters();
    void clock_freq_sweep();
    void clock_vol_envelope();

    void init_registers();
    void setup_sdl();

    void update_reg_NRx0(int channel, u8 data);
    void update_reg_NRx1(int channel, u8 data);
    void update_reg_NRx2(int channel, u8 data);
    void update_reg_NRx3(int channel, u8 data);
    void update_reg_NRx4(int channel, u8 data);

    void trigger_channel(int channel);
};

#endif