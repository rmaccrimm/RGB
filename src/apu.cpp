#include "apu.h"
#include "registers.h"
#include "util.h"
#include <iostream>
#include <cassert>
#include <algorithm>


APU::APU() : 
    clock{0},
    frame_clock{0},
    audio_sampling_clock{0},
    frame_step{0}, 
    master_enable{0}, 
    volume_left{0}, 
    volume_right{0},
    wave_RAM_pos{0},
    SQUARE_WAVEFORM{0b00000001, 0b10000001, 0b10000111, 0b01111110},
    CPU_FREQUENCY{4194304},
    AUDIO_SAMPLE_RATE{48000},
    AMPLITUDE{200},
    LFSR{0},
    right_channel_buffer(0x8000, 4194304.0 / 4.0, 48000.0),
    left_channel_buffer(0x8000, 4194304.0 / 4.0, 48000.0)
{
    init_registers();
    wave_pattern_RAM.resize(16, 0);
    right.resize(4096, 0);
    left.resize(4096, 0);
    output_buffer.resize(2 * 4096, 0);

    for (auto &ch: channels) {
        memset(&ch, 0, sizeof(Channel));
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
        // Clearing master enable disables writes to APU
        return;
    }
    
    if (addr == reg::NR52) {
        // Clearing master enable bit immediately clears all registers and flags
        master_enable = utils::bit(data, 7);
        if (!master_enable) {
            reset();
        }
        // bits 0 - 3 read-only
        data &= 0xa0;
        data |= (registers[addr] & 0xf);
    }
    else if (addr == reg::NR50) {
        // Control register
        enable_left = utils::bit(data, 7);
        enable_right = utils::bit(data, 3);
        volume_left = (data >> 4) & 7;
        volume_right = data & 7;
    }
    else if (addr == reg::NR51) {
        // Left/Right output control register
        for (int i = 0; i < 4; i++) {
            channels[i].output_right = utils::bit(data, i);
            channels[i].output_left = utils::bit(data, 4 + i);
        }
    }
    else {
        // Parse register address for channel control registers
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
    // Zero all registers
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
    for (int i = 0; i < (cycles / 4); i++) {
        clock += 4;
        frame_clock += 4;
        audio_sampling_clock += 4;

        clock_waveform_generators();

        // Frame sequencer updates at 2^9 Hz, which means 1 tick per 2^13 cpu cycles
        if (frame_clock >= 0x2000)
        {
            frame_clock -= 0x2000;
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
}

void APU::clock_waveform_generators()
{
    /*  Channels 1 and 2 contain waveform generators that produce square waves with 4 available 
        duty cycles. Each duty cycle has 8 steps. When each channel's waveform timer reaches 
        the set period, it moves to the next step and resets the timer. 
        Channel 3 outputs samples from wave pattern RAM and has a maximum period of 1024 M-cycles.
    */
    for (int i = 0; i < 4; i++) {
        auto &ch = channels[i];
        /* Timers are clocked every 4 cpu cycles (1 M-cycle) and have a maximum period of 
           2048 M-cycle
        */
        int period;
        if (i <= 1) {
            period = 4 * (0x800 - ch.frequency);
        }
        else if (i == 2) {
            period = 2 * (0x800 - ch.frequency);
        }
        else if (i == 3) {
            period = ch.frequency;
        }
        ch.waveform_clock += 4;
        if (ch.waveform_clock >= period) {
            if (i <= 1) {
                ch.waveform_step++;
                ch.waveform_step %= 8;
                ch.current_sample = (SQUARE_WAVEFORM[ch.duty] >> ch.waveform_step) & 1 ? 1 : 0;
            }
            else if (i == 2) {
                wave_RAM_pos++;
                wave_RAM_pos %= 32;
                // Even samples taken from upper 4 bits, odd from lower
                int shift = (wave_RAM_pos & 1 == 0 ? 4 : 0);
                int sample = (wave_pattern_RAM[wave_RAM_pos / 2] >> shift) & 0xf;
                if (ch.output_shift == 0) {
                    ch.volume = 0;
                }
                else {
                    ch.volume = sample >> (ch.output_shift - 1);
                }
                // Waveform is not used for channel 3 so output is determined only by volume
                ch.current_sample = 1;
            }
            else if (i == 3) {
                // Pseudo random sample generation
                u16 prev = LFSR & 0x7fff;
                int xor = (prev & 1) ^ ((prev >> 1) & 1);
                LFSR = (xor << 14) | ((prev >> 1) & 0x3fff);
                if (ch.width_mode) {
                    // set bit 6 with xor value
                    u16 mask = ~(0x40);
                    LFSR = (LFSR & mask) | (xor << 6);
                }
                ch.current_sample = !(LFSR & 1);
            }
            ch.waveform_clock -= period;
        }
    }
    // Left and right channel buffers contain full 1-Mhz (2^20) sampled audio 
    i16 l_sample = 0;
    i16 r_sample = 0;
    if (master_enable) {
        for (int i = 0; i < 4; i++) {
            if (channels[i].playing) {
                if (channels[i].output_left) {
                    l_sample += channels[i].current_sample * channels[i].volume * AMPLITUDE;
                }
                if (channels[i].output_right) {
                    r_sample += channels[i].current_sample * channels[i].volume * AMPLITUDE;
                }
            }
        }
    }
    left_channel_buffer.write(l_sample);
    right_channel_buffer.write(r_sample);
}

void APU::clock_length_counters()
{
    // Length counters disable a channel when 0 is reached
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
    /*  Volume envelopes can set the volume to 0 but do not disable the channel (clear the 
        playing flag)
    */
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
    auto &ch = channels[0];
    if (ch.freq_sweep_enable && (ch.freq_sweep_period != 0))
    {
        ch.sweep_clock++;
        if ((ch.sweep_clock % ch.freq_sweep_period) == 0) {
            int freq = shift_frequency();
            if ((freq >= 0) && (freq < 0x800) && (ch.freq_shift != 0)) {
                ch.frequency = freq;
                registers[reg::NR13] = freq & 0xff;
                u8 nr14 = registers[reg::NR14];
                registers[reg::NR14] = (nr14 & (0x1f << 3)) | ((freq >> 8) & 7);
                /*  Perform the calculation and overflow check a second time, but don't write the 
                    value back to registers (can disable channel though)
                */
                shift_frequency();
            }
        }
    }
}

int APU::shift_frequency()
{
    /*  When the new frequency is greater than 2047, channel is disabled. If it is negative, nothing
        happens
    */
    auto &ch = channels[0];
    int freq = ch.frequency;
    int df = (freq >> ch.freq_shift);
    freq += (ch.increase_freq ? df : -df);
    if (freq >= 0x800) {
        ch.playing = false;
        update_status();
    }
    return freq;
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
    Channel &ch = channels[channel_num];
    if (channel_num == 0) {
        // Frequency shift registers, only used by channel 1
        ch.freq_shift = data & 7;
        ch.increase_freq = !utils::bit(data, 3);
        ch.freq_sweep_period = (data >> 4) & 7;
    }
    else if (channel_num == 2) {
        // Channel 3 has an additional enable/disable
        ch.playing = utils::bit(data, 7);
        update_status();
    }
}

void APU::update_reg_NRx1(int channel_num, u8 data)
{
    // Update duty cycle and length counters
    Channel &ch = channels[channel_num];
    if (channel_num <= 1) {
        // Duty cycle used by channels 1 and 2 
        ch.duty = (data >> 6) & 3;
    }
    if (channel_num == 2) {
        // Channel 3 uses an 8 bit length counter
        ch.sound_length = data;
        ch.length_counter = 256 - data;
    }
    else {
        // Other channels use a 6 bit length counter
        data &= 0x3f;
        ch.sound_length = data;
        ch.length_counter = 64 - data;
    }
}

void APU::update_reg_NRx2(int channel_num, u8 data)
{
    Channel &ch = channels[channel_num];
    if (channel_num == 2) {
        ch.output_shift = (data >> 5) & 0x3;
    }
    else {
        ch.initial_volume = (data >> 4) & 0xf;
        ch.increase_volume = utils::bit(data, 3);
        ch.volume_sweep_time = data & 7;
    }
    
    /*  Each channel's DAC is controlled by the upper 5 bits of NRx2. Writing all zeroes to these
        bits disables the DAC and channel. Writing a non-zero enables the DAC again but not the 
        channel
    */
    if (((data >> 3) & 0x1f) == 0) {
        ch.DAC_enabled = false;
        ch.playing = false;
        update_status();
    }
    else {
        ch.DAC_enabled = true;
    }
}

void APU::update_reg_NRx3(int channel_num, u8 data)
{
    auto &ch = channels[channel_num];
    if (channel_num == 4) {
        // Shift clock frequency
        int shift = (data >> 4) & 0xf;
        int mult = data & 7;
        if (mult == 0) {
            mult = 1;
        }
        else {
            mult *= 2;
        }
        ch.frequency = (mult * 4) << (shift + 1);
        ch.width_mode = utils::bit(data, 3);
    }
    else {
        /*  Update lower 8 bits of channel "frequency". This is really the starting value for the 
            timers that drive each channel's waveform generator, outputing a pulse when it reaches 0
        */
        int freq = ch.frequency;
        freq = (freq & (7 << 8)) | data;
        ch.frequency = freq;    
    }
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
    /*  Triggering a channel causes the following (from Pan Docs):
        - channel is enabled
        - if length counter is 0, reload 
        - Frequency timer (waveform clock) is reloaded (set to 0)
        - Volume envelope timer is reloaded (set to 0)
        - Channel volume is reloaded from NRx2 register (inital volume)
        - Several things happen for channel 1's frequency sweep
        - Channel 4's LFSR bits are all set to 1
        - Channel 3's position is set to 0 but sample buffer is not refilled
        - If channel's DAC is disabled, channel is re-disabled at end 
    */
    Channel &ch = channels[channel_num];
    if (ch.length_counter == 0) {
        ch.length_counter = channel_num == 2 ? 256 : 64;
    }
    ch.volume = ch.initial_volume;
    ch.volume_clock = 0;
    ch.waveform_clock = 0;

    if (channel_num == 0) {
        ch.sweep_clock = 0;
        ch.freq_sweep_enable = (ch.freq_sweep_period != 0) || (ch.freq_shift != 0);
        if (ch.freq_shift != 0) {
            // frequency calculation and overflow check performed immediately
            int freq = shift_frequency();
        }
    }
    else if (channel_num == 2) {
        wave_RAM_pos = 0;
    }
    else if (channel_num == 3) {
        LFSR = 0x7fff;
    }
    ch.playing = ch.DAC_enabled;
    update_status();
}

void APU::start()
{
    SDL_PauseAudioDevice(device_id, 0);
}

int APU::flush_buffer()
{   
    int size;
    right_channel_buffer.downsample(right, size);
    left_channel_buffer.downsample(left, size);
    // std::cout << SDL_GetQueuedAudioSize(device_id) << std::endl;
    for (int i = 0; i < right.size(); i++) {
        output_buffer[2*i] = left[i];
        output_buffer[2*i + 1] = right[i];
    }
    SDL_QueueAudio(device_id, output_buffer.data(), 2 * size * sizeof(i16));   
    return SDL_GetQueuedAudioSize(device_id);
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

    SDL_AudioSpec obtained;
    SDL_zero(spec);

    spec.freq = AUDIO_SAMPLE_RATE;
    spec.format = AUDIO_S16SYS;
    spec.channels = 2;
    spec.samples = AUDIO_SAMPLE_RATE / 60;
    spec.callback = NULL;
    spec.userdata = NULL;

    if (SDL_InitSubSystem(SDL_INIT_AUDIO) != 0)
    {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
    }
    
    device_id = SDL_OpenAudioDevice(NULL, 0, &spec, &obtained, 0);
    spec = obtained;
    SDL_Log("Audio rate: %d", spec.freq);
    if (device_id == 0)
    {
        SDL_Log("Failed to open audio: %s", SDL_GetError());
    }
}