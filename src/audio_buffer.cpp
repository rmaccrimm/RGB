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

    // Cubic interpolation
    /*for (int i = 1; i < dest.size() + 1; i++) {
        int k = i * ratio;
        double m0 = 0.5 * (src[k+1] - src[k-1]);
        double m1 = 0.5 * (src[k+2] - src[k]);

        double t = (((double)i * ratio) - k);
        double t2 = t*t;
        double t3 = t*t*t;
        double h00 = 2*t3 - 3*t2 + 1;
        double h01 = -2*t3 + 3*t2;
        double h10 = t3 - 2*t2 + t;
        double h11 = t3 - t2;

        dest[i-1] = h00*(double)src[k] + h10*m0 + h01*(double)src[k+1] + h11*m1;
    }*/
}
