#include "audio_buffer.h"

AudioBuffer::AudioBuffer(int size, double sample_rate, double target_rate)
{
    buffer.resize(size, 0);
    position = buffer.begin();    
    next_sample = buffer.begin();
    bool wrap = false;
    ratio = sample_rate / target_rate;
    sample_position = 0;
}

void AudioBuffer::write(i16 x)
{
    *position = x;
    position++;
    if (position == buffer.end()) {
        position = buffer.begin();
        wrap = true;
    }
}

void AudioBuffer::downsample(std::vector<i16> &dest, int &size)
{
    int i = 0; 
    while (true) {
        if (!wrap && ((next_sample + 2) > position)) {
            break;
        }
        if (i >= dest.size()) {
            break;
        }
        dest[i] = (interpolate(next_sample, sample_position - (int)sample_position));
        i++;
        sample_position += ratio;
        if (sample_position > buffer.size()) {
            sample_position -= buffer.size();
            wrap = false;
        }
        next_sample = buffer.begin() + (int)sample_position;
    }
    size = i;
}

i16 AudioBuffer::interpolate(std::vector<i16>::iterator lower, double dist) 
{
    // Nearest neighbour sampling
    auto it = lower + (int)(dist + 0.5);
    if (it >= buffer.end()) {
        it = buffer.begin() + (int)(it - buffer.end());
    }
    return *it;
}
