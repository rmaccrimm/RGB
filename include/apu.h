#ifndef APU_H
#define APU_H

#include "definitions.h"
#include "signal_processing.h"
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

    void flush_buffer();

private:

    struct Channel
    {
        bool playing;
        bool output_left;
        bool output_right;
        int length_counter;
        int sound_length;
        bool decrement_counter;
        bool trigger;
        int duty;
        int frequency;
        int initial_volume;
        bool increase_volume;
        int volume_sweep_time;
        int volume;
        int volume_clock;
        int initial_freq;
        bool increase_freq;
        int freq_sweep_enable;
        int freq_sweep_time;
        int freq_shift;
        int freq_clock;
        int waveform_clock;
        int waveform_step;
        int current_sample;
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
    unsigned int frame_clock;
    unsigned int audio_sampling_clock;
    unsigned int frame_step;

    SDL_AudioDeviceID device_id;
    SDL_AudioSpec spec;

    AudioBuffer right_channel_buffer;
    AudioBuffer left_channel_buffer;

    std::vector<i16> right;
    std::vector<i16> left;
    std::vector<i16> output_buffer;

    void reset();

    void clock_waveform_generators();
    void clock_length_counters();
    void clock_freq_sweep();
    void clock_vol_envelope();

    void init_registers();
    void setup_sdl();

    void update_status();
    void update_reg_NRx0(int channel, u8 data);
    void update_reg_NRx1(int channel, u8 data);
    void update_reg_NRx2(int channel, u8 data);
    void update_reg_NRx3(int channel, u8 data);
    void update_reg_NRx4(int channel, u8 data);
    void trigger_channel(int channel);

    void append_audio_sample();

    const unsigned int CPU_FREQUENCY;
    const unsigned int AUDIO_SAMPLE_RATE;
    const u8 SQUARE_WAVEFORM[4];
};

#endif