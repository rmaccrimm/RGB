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

APU::APU() : 
    clock{0}, 
    audio_sampling_clock{0},
    frame_step{0}, 
    master_enable{0}, 
    volume_left{0}, 
    volume_right{0},
    SQUARE_WAVEFORM{0b00000001, 0b10000001, 0b10000111, 0b01111110},
    CPU_FREQUENCY{4194304},
    AUDIO_SAMPLE_RATE{48000},
    buffer_ind{0}
{
    init_registers();
    wave_pattern_RAM.resize(16, 0);
    audio_buffer.resize(1024, 0);

    for (auto &channel : channels)
    {
        channel.playing = false;
        channel.enable = false;
        channel.output_left = false;
        channel.output_right = false;
        channel.initial_volume = 0;
        channel.length_counter = 0;
        channel.volume_clock = 0;
        channel.freq_clock = 0;
        channel.waveform_clock = 0;
        channel.waveform_step = 0;
    }

    setup_sdl();
}

APU::~APU()
{
    SDL_CloseAudioDevice(device_id);
    SDL_Quit();
}

u8 APU::read(u16 addr) 
{
    if (addr >= 0xff30) {
        return wave_pattern_RAM[addr - 0xff30];
    }
    else if (unused_addr[addr]) {
        return 0xff;
    }
    else {
        return registers[addr] | read_masks[addr];
    }
}

void APU::write(u16 addr, u8 data) 
{
    if (addr >= 0xff30) {
        wave_pattern_RAM[addr - 0xff30] = data;
        return;
    }
    else if (unused_addr[addr]) {
        return;
    }
    
    if (!master_enable && addr != reg::NR52) {
        return;
    }

    if (addr == reg::NR52) {
        master_enable = utils::bit(data, 7);
        if (!master_enable) {
            reset();
        }
        // bits 0 - 3 read-only
        data &= 0xa0;
        data |= (registers[addr] & 0xf);
    }
    else if (addr == reg::NR50) {
        enable_left = utils::bit(data, 7);
        enable_right = utils::bit(data, 3);
        volume_left = (data >> 4) & 7;
        volume_right = data & 7;
    }
    else if (addr == reg::NR51) {
        for (int i = 0; i < 4; i++) {
            channels[i].output_right = utils::bit(data, i);
            channels[i].output_left = utils::bit(data, 4 + i);
        }
    }
    else {
        // Parse register address
        int reg = ((addr & 0xff) - 0x10) % 5;
        int ch = ((addr & 0xff) - 0x10) / 5;
        if (reg == 0) { 
            update_reg_NRx0(ch, data);
        }
        else if (reg == 1) {
            update_reg_NRx1(ch, data);
        }
        else if (reg == 2) {
            update_reg_NRx2(ch, data);
        }
        else if (reg == 3) {
            update_reg_NRx3(ch, data);
        }
        else if (reg == 4) {
            update_reg_NRx4(ch, data);
        }
    }
    registers[addr] = data;
}

void APU::reset()
{
    for (auto &p: registers) {
        p.second = 0;
    }
    for (int i = 0; i < 4; i++) {
        update_reg_NRx0(i, 0);
        update_reg_NRx1(i, 0);
        update_reg_NRx2(i, 0);
        update_reg_NRx3(i, 0);
        update_reg_NRx4(i, 0);
    }
}


void APU::step(int cycles)
{
    clock += cycles;
    audio_sampling_clock += cycles;

    for (auto &ch: channels) {
        int period = 4 * (0x800 - ch.frequency);
        ch.waveform_clock += cycles;
        while (ch.waveform_clock >= period) {
            ch.waveform_step++;
            ch.waveform_step %= 8;
            ch.current_sample = (SQUARE_WAVEFORM[ch.duty] >> ch.waveform_step) & 1;
        }
    }

    int clocks_per_sample = CPU_FREQUENCY / AUDIO_SAMPLE_RATE;
    if (audio_sampling_clock >= clocks_per_sample) {
        append_audio_sample();
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
            clock_freq_sweep();
        }
        if (frame_step == 7)
        {
            // Volume counter clocked at 64 Hz
            clock_vol_envelope();
        }
        update_status();
    }
}

void APU::update_status()
{
    u8 status = registers[reg::NR52];
    for (int i = 0; i < 4; i++) {
        status = utils::set_cond(status, i, channels[i].playing);
    }
    registers[reg::NR52] = status;
}

void APU::update_reg_NRx0(int channel_num, u8 data)
{
    if (channel_num == 0) {
        Channel &ch = channels[channel_num];
        ch.freq_shift = data & 7;
        ch.increase_freq = !utils::bit(data, 3);
        ch.freq_sweep_time = (data >> 4) & 7;
    }
}

void APU::update_reg_NRx1(int channel_num, u8 data)
{
    Channel &ch = channels[channel_num];
    if (channel_num <= 1) {
        ch.duty = (data >> 6) & 3;
    }
    if (channel_num == 2) {
        ch.sound_length = data;
        ch.length_counter = 256 - data;
    }
    else {
        data &= 0x3f;
        ch.sound_length = data;
        ch.length_counter = 64 - data;
    }
}

void APU::update_reg_NRx2(int channel_num, u8 data)
{
    Channel &ch = channels[channel_num];
    ch.initial_volume = (data >> 4) & 0xf;
    ch.increase_volume = utils::bit(data, 3);
    ch.volume_sweep_time = data & 7;
    if (((data >> 3) & 0x1f) == 0) {
        ch.playing = false;
        update_status();
    }
}

void APU::update_reg_NRx3(int channel_num, u8 data)
{
    int freq = channels[channel_num].frequency;
    freq = (freq & (7 << 8)) | data;
    channels[channel_num].frequency = freq;    
}

void APU::update_reg_NRx4(int channel_num, u8 data)
{
    if (utils::bit(data, 7)) {
        trigger_channel(channel_num);
    }

    Channel &ch = channels[channel_num];
    ch.decrement_counter = utils::bit(data, 6);
    if (channel_num != 3) { 
        // set higher 3 bits of frequency
        int freq = ch.frequency;
        freq = ((data & 7) << 8) | (freq & 0xff);
        ch.frequency = freq;
    }
}

void APU::trigger_channel(int channel_num)
{
    Channel &ch = channels[channel_num];
    if (ch.length_counter == 0) {
        ch.length_counter = channel_num == 2 ? 256 : 64;
    }
    if (channel_num <= 1) {
        ch.volume = ch.initial_volume;
    }
    ch.volume_clock = 0;
    ch.freq_clock = 0;
    ch.playing = true;
}

void APU::clock_length_counters()
{
    for (auto & ch: channels) {
        if (ch.decrement_counter && (ch.length_counter > 0)) {
            ch.length_counter--;
        }
        if (ch.length_counter == 0) {
            ch.playing = false;
        }
    }
}

void APU::clock_vol_envelope()
{
    for (auto &ch: channels) {
        ch.volume_clock++;
        if (ch.volume_sweep_time != 0) {
            if ((ch.volume_clock % ch.volume_sweep_time) == 0) {
                if (ch.increase_volume) {
                    ch.volume = std::min(15, ch.volume + 1);
                }
                else{
                    ch.volume = std::max(0, ch.volume - 1);
                }
            }
        }
    }
}

void APU::clock_freq_sweep()
{
    /*if (!channel_1.freq_sweep_enable)
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
    }*/
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
        if (master_enable) {
            for (int j = 0; j < 2; j++) {
                _stream[2 * i] += channels[j].output_left ? sample_channel(j) : 0;
                _stream[2 * i + 1] += channels[j].output_right ? sample_channel(j) : 0;
            }
        }
        t += dt;
    }
}

void APU::append_audio_sample()
{
    audio_buffer[buffer_ind] = 0;
    for (int i = 0; i < 2; i++) {
        audio_buffer[buffer_ind] += channels[i].current_sample;
    }
    buffer_ind++;
}

void APU::flush_buffer()
{
    
}

int APU::sample_channel(int channel_num)
{
    Channel &ch = channels[channel_num];
    if (channel_num <= 1) {
        if (ch.playing) {
            return square_wave(t, ch.frequency, ch.volume * AMPLITUDE, ch.duty);
        }
        else {
            return 0;
        }
    }
    else {
        return 0;
    }
}

void APU::init_registers()
{
    read_masks[reg::NR10] = 0x80;
    read_masks[reg::NR11] = 0x3f;
    read_masks[reg::NR12] = 0x00;
    read_masks[reg::NR13] = 0xff;
    read_masks[reg::NR14] = 0xbf;

    read_masks[reg::NR21] = 0x3f;
    read_masks[reg::NR22] = 0x00;
    read_masks[reg::NR23] = 0xff;
    read_masks[reg::NR24] = 0xbf;

    read_masks[reg::NR30] = 0x7f;
    read_masks[reg::NR31] = 0xff;
    read_masks[reg::NR32] = 0x9f;
    read_masks[reg::NR33] = 0xff;
    read_masks[reg::NR34] = 0xbf;

    read_masks[reg::NR41] = 0xff;
    read_masks[reg::NR42] = 0x00;
    read_masks[reg::NR43] = 0x00;
    read_masks[reg::NR44] = 0xbf;

    read_masks[reg::NR50] = 0x00;
    read_masks[reg::NR51] = 0x00;
    read_masks[reg::NR52] = 0x70;

    for (int i = 0xff10; i <= 0xff2f; i++)
    {
        unused_addr[i] = (read_masks.find(i) == read_masks.end());
    }
}

void APU::setup_sdl()
{
    for (int i = 0; i < SDL_GetNumAudioDevices(0); i++)
    {
        SDL_Log("%s", SDL_GetAudioDeviceName(i, 0));
    }

    SDL_AudioSpec spec, obtained;
    SDL_zero(spec);

    spec.freq = AUDIO_SAMPLE_RATE;
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