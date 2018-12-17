#include "apu.h"
#include "registers.h"
#include "util.h"
#include <iostream>

#define _USE_MATH_DEFINES
#include <cmath>
#include <algorithm>

int v = 0;
const Sint16 AMPLITUDE = 100;
const int FREQUENCY1 = 300;
const int FREQUENCY2 = 60;
int per_s = 48000;
int per_call = 1024;
double call_freq = (double)per_s / (double)per_call;
double dt = 1.0f / call_freq/ (double)1024.0f;
double t = 0;

APU::APU(Memory *mem) : 
    memory{mem},
    clock{0},
    frame_step{0},
    reg_nr10{mem->get_mem_reference(reg::NR10)},
    reg_nr11{mem->get_mem_reference(reg::NR11)},
    reg_nr12{mem->get_mem_reference(reg::NR12)},
    reg_nr13{mem->get_mem_reference(reg::NR13)},
    reg_nr14{mem->get_mem_reference(reg::NR14)},
    reg_nr21{mem->get_mem_reference(reg::NR21)},
    reg_nr22{mem->get_mem_reference(reg::NR22)},
    reg_nr23{mem->get_mem_reference(reg::NR23)},
    reg_nr24{mem->get_mem_reference(reg::NR24)},
    reg_nr30{mem->get_mem_reference(reg::NR30)},
    reg_nr31{mem->get_mem_reference(reg::NR31)},
    reg_nr32{mem->get_mem_reference(reg::NR32)},
    reg_nr33{mem->get_mem_reference(reg::NR33)},
    reg_nr34{mem->get_mem_reference(reg::NR34)},
    reg_nr41{mem->get_mem_reference(reg::NR41)},
    reg_nr42{mem->get_mem_reference(reg::NR42)},
    reg_nr43{mem->get_mem_reference(reg::NR43)},
    reg_nr44{mem->get_mem_reference(reg::NR44)},
    reg_nr50{mem->get_mem_reference(reg::NR50)},
    reg_nr51{mem->get_mem_reference(reg::NR51)},
    reg_nr52{mem->get_mem_reference(reg::NR52)}
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
    read_registers();  

    channel_1.volume_clock = 0;
    channel_1.volume = channel_2.initial_volume;
    channel_1.freq_clock = 0;

    channel_2.volume_clock = 0;
    channel_2.volume = channel_2.initial_volume;
}

APU::~APU() {
    SDL_CloseAudioDevice(device_id);
    SDL_Quit();
}

void APU::step(int cycles)
{
    clock += cycles;
    read_registers();

    if (channel_1.restart) {
        channel_1.freq_sweep_enable = true;
        channel_1.volume = channel_1.initial_volume;
        reg_nr14 = utils::reset(reg_nr14, 7);
    }
    if (channel_2.restart) {
        channel_2.volume = channel_2.initial_volume;
        // channel_2.restart = false;
        // reg_nr14 = utils::reset(reg_nr24, 7);
    }

    // Frame sequencer updates at 2^9 Hz, which means 1 tick per 2^13 cpu cycles 
    if (clock >= 0x2000) {
        clock -= 0x2000;

        frame_step++;
        frame_step %= 8;

        if ((frame_step & 1) == 0) {
        // Length counter clocked at 256 Hz
            clock_length_counters();
        }
        if ((frame_step == 2 || frame_step == 6)) {
        // Frequency counter clocked at 128 Hz
            clock_freq_sweep();
        } 
        if (frame_step == 7) {
        // Volume counter clocked at 64 Hz
            clock_vol_envelope();
        }

        channel_1.enable = channel_1.counter != 0 && channel_1.freq_sweep_enable;
        reg_nr52 = utils::set_cond(reg_nr52, 0, channel_1.enable);

        channel_2.enable = channel_2.counter != 0;
        reg_nr52 = utils::set_cond(reg_nr52, 1, channel_2.enable);
    }
}

void APU::clock_freq_sweep()
{
    channel_1.freq_clock++;
    
    if (!channel_1.freq_sweep_enable) {
        return;
    }
    channel_1.enable = false;

    if (channel_1.freq_sweep_time != 0 && channel_1.freq_shift != 0) {

        if (channel_1.freq_clock % channel_1.freq_sweep_time == 0) {
            int df = (channel_1.freq >> channel_1.freq_shift) & 0x7ff;
            channel_1.freq += (channel_1.increase_freq ? df : -df);
            
            if (channel_1.freq < 0) {
                channel_1.freq += df;
            }
            else if (channel_1.freq >= 0x800) {
                channel_1.freq_sweep_enable = false;
            }
            else {
                reg_nr13 = channel_1.freq & 0xff;
                reg_nr14 = (reg_nr14 & (0x1f << 3)) | ((channel_1.freq >> 8) & 7);
            }
        }
    }
    channel_1.enable = true;
}

void APU::clock_vol_envelope()
{
    channel_1.volume_clock++;
    channel_2.volume_clock++;

    if (channel_1.volume_sweep_time != 0) {
        if (channel_1.volume_clock % channel_1.volume_sweep_time == 0) {
            if (channel_1.increase_volume) {
                channel_1.volume = std::min(15, channel_1.volume - 1);
            }
            else {
                channel_1.volume = std::max(0, channel_1.volume - 1);
            }
        }
    }

    if (channel_2.volume_sweep_time != 0) {
        if (channel_2.volume_clock % channel_2.volume_sweep_time == 0) {
            if (channel_2.increase_volume) {
                channel_2.volume = std::min(15, channel_2.volume - 1);
            }
            else {
                channel_2.volume = std::max(0, channel_2.volume - 1);
            }
        }
    }
}

void APU::clock_length_counters()
{
    if (channel_1.decrement_counter && channel_1.counter > 0) {
        reg_nr11 = (reg_nr11 && (7 << 5)) && ((reg_nr11 - 1) & 0x1f);
        channel_1.counter = reg_nr11;
    }
    if (channel_2.decrement_counter && channel_2.counter > 0) {
        reg_nr21 = (reg_nr21 && (7 << 5)) && ((reg_nr21 - 1) & 0x1f);
        channel_2.counter = reg_nr21;
    }


}

int APU::square_wave(double t, double freq, int amp, int duty)
{
    freq = 131072.0 / (0x800 - freq);
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
    Sint16* _stream = (Sint16*)stream;
    for (int i = 0; i < len/4; i++) {
        _stream[2*i] = 0;
        _stream[2*i + 1] = 0;
        if (sound_control.master_enable) {
            _stream[2*i] += sound_control.channel_1_left ?  sample_channel_1() : 0;
            _stream[2*i] += sound_control.channel_2_left ?  sample_channel_2() : 0;
            _stream[2*i] += sound_control.channel_3_left ?  sample_channel_3() : 0;
            _stream[2*i] += sound_control.channel_4_left ?  sample_channel_4() : 0;
            _stream[2*i + 1] += sound_control.channel_1_right ? sample_channel_1() : 0;
            _stream[2*i + 1] += sound_control.channel_2_right ? sample_channel_2() : 0;
            _stream[2*i + 1] += sound_control.channel_3_right ? sample_channel_3() : 0;
            _stream[2*i + 1] += sound_control.channel_4_right ? sample_channel_4() : 0;
        }
        t += dt;
    }
}

int APU::sample_channel_1()
{
    if (channel_1.enable) {
        return square_wave(t, channel_1.freq, channel_1.volume * AMPLITUDE, channel_1.duty);
    }
    else {
        return 0;
    }
}

int APU::sample_channel_2()
{
    if (channel_2.enable) {
        return square_wave(t, channel_2.frequency, channel_2.volume * AMPLITUDE, channel_2.duty);
    }
    else {
        return 0;
    }
}

int APU::sample_channel_3()
{
    return 0;
}

int APU::sample_channel_4()
{
    return 0;
}

void APU::read_registers()
{
    sound_control.master_enable = utils::bit(reg_nr52, 7);
    sound_control.volume_left = (reg_nr50 >> 4) & 7;
    sound_control.volume_right = reg_nr50 &  7;
    
    sound_control.channel_1_right = utils::bit(reg_nr51, 0);
    sound_control.channel_2_right = utils::bit(reg_nr51, 1);
    sound_control.channel_3_right = utils::bit(reg_nr51, 2);
    sound_control.channel_4_right = utils::bit(reg_nr51, 3);
    sound_control.channel_1_left = utils::bit(reg_nr51, 4);
    sound_control.channel_2_left = utils::bit(reg_nr51, 5);
    sound_control.channel_3_left = utils::bit(reg_nr51, 6);
    sound_control.channel_4_left = utils::bit(reg_nr51, 7);
    
    u8 freq_lo = reg_nr13;
    u8 freq_hi = reg_nr14 & 7;

    channel_1.enable = false;
    channel_1.freq = ((freq_hi << 8) | freq_lo) & 0x7ff;
    channel_1.enable = true;

    channel_1.freq_shift = reg_nr10 & 7;
    channel_1.increase_freq = !utils::bit(reg_nr10, 3);
    channel_1.freq_sweep_time = (reg_nr10 >> 4) & 7;

    channel_1.decrement_counter = utils::bit(reg_nr14, 6);
    channel_1.duty = (reg_nr11 >> 6) & 3;
    channel_1.initial_volume = (reg_nr12 >> 4 ) & 0xf;
    channel_1.increase_volume = utils::bit(reg_nr12, 3);
    channel_1.volume_sweep_time = reg_nr12 & 7; 
    channel_1.restart = utils::bit(reg_nr14, 7);

    freq_lo = reg_nr23;
    freq_hi = reg_nr24 & 7;
    channel_2.frequency = (freq_hi << 8) | freq_lo & 0x7ff;
    channel_2.decrement_counter = utils::bit(reg_nr24, 6);
    channel_2.duty = (reg_nr21 >> 6) & 3;
    channel_2.initial_volume = (reg_nr22 >> 4 ) & 0xf;
    channel_2.increase_volume = utils::bit(reg_nr22, 3);
    channel_2.volume_sweep_time = reg_nr22 & 7;
    channel_2.restart = utils::bit(reg_nr24, 7);
}