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
double dt = 1.0f / call_freq / (double)1024.0f;
double t = 0;

APU::APU() : clock{0}, frame_step{0}, master_enable{0}, volume_left{0}, volume_right{0}
{
    init_registers();
    wave_pattern_RAM.resize(16, 0);

    for (auto &channel : channels)
    {
        channel.playing = false;
        channel.enable = false;
        channel.output_left = false;
        channel.output_right = false;
        channel.volume_clock = 0;
        channel.freq_clock = 0;
        channel.initial_volume = 0;
        channel.length_counter = 64;
    }
    channels[2].length_counter = 256;

    init_SDL();
}

APU::~APU()
{
    SDL_CloseAudioDevice(device_id);
    SDL_Quit();
}

u8 APU::read(u16 addr) {
    if (unused_addr[addr]) {
        return 0xff;
    }
    if (addr >= 0x30) {
        return wave_pattern_RAM[addr - 0xff30];
    }
    else {
        return registers[addr] | read_masks[addr];
    }
}

void APU::write(u16 addr, u8 data) {
    if (unused_addr[addr]) {
        return;
    }
    if (addr >= 0x30) {
        wave_pattern_RAM[addr - 0xff30] = data;
        return;
    }
    else if (addr == reg::NR50) {
        update_output_control(data);
    }
    else if (addr == reg::NR51) {
        update_LR_control(data);
    }
    else if (addr == reg::NR52) {
        master_enable = utils::bit(data, 7);
    }
    else {
        int register_num = ((addr & 0xff) - 0x10) % 5;
        int channel_num = ((addr & 0xff) - 0x10) / 5;

        if (register_num == 0) { // registers NRx0

        }
        else if (register_num == 1) { // registers NRx1
            update_length_counter(channel_num, data);
        }
        else if (register_num == 2) { // regiseter NRx2

        }
        else if (register_num == 3) { // registers NRx3

        }
    }
}

void APU::step(int cycles)
{
    clock += cycles;

    if (memory->audio_trigger[0])
    {
        channel_1.enabled = true;
        if (channel_1.length_counter == 0)
        {
            channel_1.length_counter = 64;
        }
        channel_1.volume = channel_1.initial_volume;
        memory->audio_trigger[0] = false;
    }

    if (memory->audio_trigger[1])
    {
        if (channel_2.length_counter == 0)
        {
            channel_2.length_counter = 64;
        }
        channel_2.volume = channel_2.initial_volume;
        channel_2.enabled = true;
        memory->audio_trigger[1] = false;
    }

    if (memory->audio_trigger[2])
    {
        if (channel_3.length_counter == 0)
        {
            channel_3.length_counter = 256;
        }
        channel_3.enabled = true;
        memory->audio_trigger[2] = false;
    }

    if (memory->audio_trigger[3])
    {
        if (channel_4.length_counter == 0)
        {
            channel_4.length_counter = 64;
        }
        channel_4.enabled = true;
        memory->audio_trigger[3] = false;
    }

    if (memory->reload_audio_counter[0])
    {
        channel_1.length_counter = 64 - channel_1.sound_length;
        memory->reload_audio_counter[0] = false;
        // channel_1.enabled = true;
    }
    if (memory->reload_audio_counter[1])
    {
        channel_2.length_counter = 64 - channel_2.sound_length;
        memory->reload_audio_counter[1] = false;
        // channel_2.enabled = true;
    }
    if (memory->reload_audio_counter[2])
    {
        channel_3.length_counter = 256 - channel_3.sound_length;
        memory->reload_audio_counter[2] = false;
        // channel_3.enabled = true;
    }
    if (memory->reload_audio_counter[3])
    {
        channel_4.length_counter = 64 - channel_4.sound_length;
        memory->reload_audio_counter[3] = false;
        // channel_4.enabled = true;
    }

    // Frame sequencer updates at 2^9 Hz, which means 1 tick per 2^13 cpu cycles
    if (clock >= 0x2000)
    {
        clock -= 0x2000;

        frame_step++;
        frame_step %= 8;

        if ((frame_step & 1) == 0)
        {
            // Length counter clocked at 256 Hz
            clock_length_counters();
        }
        if ((frame_step == 2 || frame_step == 6))
        {
            // Frequency counter clocked at 128 Hz
            // clock_freq_sweep();
        }
        if (frame_step == 7)
        {
            // Volume counter clocked at 64 Hz
            clock_vol_envelope();
        }

        reg_nr52 = utils::set_cond(reg_nr52, 0, channel_1.enabled);
        reg_nr52 = utils::set_cond(reg_nr52, 1, channel_2.enabled);
        reg_nr52 = utils::set_cond(reg_nr52, 2, channel_3.enabled);
        reg_nr52 = utils::set_cond(reg_nr52, 3, channel_4.enabled);
    }
}

void APU::clock_length_counters()
{
    if (channel_1.decrement_counter && (channel_1.length_counter > 0))
    {
        channel_1.length_counter--;
    }
    if (channel_1.length_counter == 0)
    {
        channel_1.enabled = false;
    }
    if (channel_2.decrement_counter && (channel_2.length_counter > 0))
    {
        channel_2.length_counter--;
    }
    if (channel_2.length_counter == 0)
    {
        channel_2.enabled = false;
    }
    if (channel_3.decrement_counter && (channel_3.length_counter > 0))
    {
        channel_3.length_counter--;
    }
    if (channel_3.length_counter == 0)
    {
        channel_3.enabled = false;
    }
    if (channel_4.decrement_counter && (channel_4.length_counter > 0))
    {
        channel_4.length_counter--;
    }
    if (channel_4.length_counter == 0)
    {
        channel_4.enabled = false;
    }
}

void APU::clock_freq_sweep()
{
    channel_1.freq_clock++;

    if (!channel_1.freq_sweep_enable)
    {
        return;
    }

    if ((channel_1.freq_sweep_time != 0) && (channel_1.freq_shift != 0))
    {
        if ((channel_1.freq_clock % channel_1.freq_sweep_time) == 0)
        {
            int df = (channel_1.freq >> channel_1.freq_shift) & 0x7ff;
            channel_1.freq += (channel_1.increase_freq ? df : -df);

            if (channel_1.freq < 0)
            {
                channel_1.freq += df;
            }
            else if (channel_1.freq >= 0x800)
            {
                channel_1.freq_sweep_enable = false;
            }
            else
            {
                reg_nr13 = channel_1.freq & 0xff;
                reg_nr14 = (reg_nr14 & (0x1f << 3)) | ((channel_1.freq >> 8) & 7);
            }
        }
    }
}

void APU::clock_vol_envelope()
{
    channel_1.volume_clock++;
    channel_2.volume_clock++;

    if (channel_1.volume_sweep_time != 0)
    {
        if (channel_1.volume_clock % channel_1.volume_sweep_time == 0)
        {
            if (channel_1.increase_volume)
            {
                channel_1.volume = std::min(15, channel_1.volume - 1);
            }
            else
            {
                channel_1.volume = std::max(0, channel_1.volume - 1);
            }
        }
    }

    if (channel_2.volume_sweep_time != 0)
    {
        if (channel_2.volume_clock % channel_2.volume_sweep_time == 0)
        {
            if (channel_2.increase_volume)
            {
                channel_2.volume = std::min(15, channel_2.volume - 1);
            }
            else
            {
                channel_2.volume = std::max(0, channel_2.volume - 1);
            }
        }
    }

    if (channel_1.volume == 0)
    {
        channel_1.enabled = false;
    }
    if (channel_2.volume == 0)
    {
        channel_2.enabled = false;
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
    static_cast<APU *>(userdata)->audio_callback(stream, len);
}

void APU::audio_callback(Uint8 *stream, int len)
{
    Sint16 *_stream = (Sint16 *)stream;
    for (int i = 0; i < len / 4; i++)
    {
        _stream[2 * i] = 0;
        _stream[2 * i + 1] = 0;
        if (sound_control.master_enable)
        {
            _stream[2 * i] += sound_control.channel_1_left ? sample_channel_1() : 0;
            _stream[2 * i] += sound_control.channel_2_left ? sample_channel_2() : 0;
            _stream[2 * i] += sound_control.channel_3_left ? sample_channel_3() : 0;
            _stream[2 * i] += sound_control.channel_4_left ? sample_channel_4() : 0;
            _stream[2 * i + 1] += sound_control.channel_1_right ? sample_channel_1() : 0;
            _stream[2 * i + 1] += sound_control.channel_2_right ? sample_channel_2() : 0;
            _stream[2 * i + 1] += sound_control.channel_3_right ? sample_channel_3() : 0;
            _stream[2 * i + 1] += sound_control.channel_4_right ? sample_channel_4() : 0;
        }
        t += dt;
    }
}

int APU::sample_channel_1()
{
    if (channel_1.enabled)
    {
        return square_wave(t, channel_1.freq, channel_1.volume * AMPLITUDE, channel_1.duty);
    }
    else
    {
        return 0;
    }
}

int APU::sample_channel_2()
{
    if (channel_2.enabled)
    {
        return square_wave(t, channel_2.frequency, channel_2.volume * AMPLITUDE, channel_2.duty);
    }
    else
    {
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
    sound_control.volume_right = reg_nr50 & 7;

    sound_control.channel_1_right = utils::bit(reg_nr51, 0);
    sound_control.channel_2_right = utils::bit(reg_nr51, 1);
    sound_control.channel_3_right = utils::bit(reg_nr51, 2);
    sound_control.channel_4_right = utils::bit(reg_nr51, 3);
    sound_control.channel_1_left = utils::bit(reg_nr51, 4);
    sound_control.channel_2_left = utils::bit(reg_nr51, 5);
    sound_control.channel_3_left = utils::bit(reg_nr51, 6);
    sound_control.channel_4_left = utils::bit(reg_nr51, 7);

    channel_1.sound_length = reg_nr11 & 0x3f;
    channel_2.sound_length = reg_nr21 & 0x3f;
    channel_3.sound_length = reg_nr31;
    channel_4.sound_length = reg_nr41 & 0x3f;

    channel_1.decrement_counter = utils::bit(reg_nr14, 6);
    channel_2.decrement_counter = utils::bit(reg_nr24, 6);
    channel_3.decrement_counter = utils::bit(reg_nr34, 6);
    channel_4.decrement_counter = utils::bit(reg_nr44, 6);

    u8 freq_lo = reg_nr13;
    u8 freq_hi = reg_nr14 & 7;
    channel_1.freq = ((freq_hi << 8) | freq_lo) & 0x7ff;
    channel_1.freq_shift = reg_nr10 & 7;
    channel_1.increase_freq = !utils::bit(reg_nr10, 3);
    channel_1.freq_sweep_time = (reg_nr10 >> 4) & 7;

    channel_1.duty = (reg_nr11 >> 6) & 3;
    channel_1.initial_volume = (reg_nr12 >> 4) & 0xf;
    channel_1.increase_volume = utils::bit(reg_nr12, 3);
    channel_1.volume_sweep_time = reg_nr12 & 7;

    freq_lo = reg_nr23;
    freq_hi = reg_nr24 & 7;
    channel_2.frequency = (freq_hi << 8) | freq_lo & 0x7ff;

    channel_2.duty = (reg_nr21 >> 6) & 3;
    channel_2.initial_volume = (reg_nr22 >> 4) & 0xf;
    channel_2.increase_volume = utils::bit(reg_nr22, 3);
    channel_2.volume_sweep_time = reg_nr22 & 7;
}

void APU::init_registers()
{
    read_masks[reg::NR10] = 0b10000000;
    read_masks[reg::NR11] = 0b00111111;
    read_masks[reg::NR12] = 0;
    read_masks[reg::NR13] = 0;
    read_masks[reg::NR14] = 0b10111111;
    read_masks[reg::NR21] = 0b00111111;
    read_masks[reg::NR22] = 0;
    read_masks[reg::NR23] = 0;
    read_masks[reg::NR24] = 0b10111111;
    read_masks[reg::NR30] = 0b01111111;
    read_masks[reg::NR31] = 0;
    read_masks[reg::NR32] = 0b10011111;
    read_masks[reg::NR33] = 0;
    read_masks[reg::NR34] = 0b10111111;
    read_masks[reg::NR41] = 0b11000000;
    read_masks[reg::NR42] = 0;
    read_masks[reg::NR43] = 0;
    read_masks[reg::NR44] = 0b10111111;
    read_masks[reg::NR50] = 0;
    read_masks[reg::NR51] = 0;
    read_masks[reg::NR52] = 0b01110000;

    for (int i = 0xff10; i <= 0xff2f; i++)
    {
        unused_addr[i] = read_masks.find(i) == read_masks.end();
    }
}

void APU::init_SDL()
{
    for (int i = 0; i < SDL_GetNumAudioDevices(0); i++)
    {
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

    if (SDL_InitSubSystem(SDL_INIT_AUDIO) != 0)
    {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
    }
    device_id = SDL_OpenAudioDevice(NULL, 0, &spec, &obtained, 0);
    if (device_id == 0)
    {
        SDL_Log("Failed to open audio: %s", SDL_GetError());
    }
}