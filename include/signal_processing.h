#include "definitions.h"
#include <vector>

class AudioBuffer 
// Circular, fixed-size buffer for storing audio samples
{
public:
    AudioBuffer(int size, double sample_rate, double target_rate);

    void write(i16 x);

    // Resample the audio buffer to the target rate, filling dest with as much data as is available.
    void downsample(std::vector<i16> &dest);

private:
    // Interpolate between pos and pos + 1 at given distance from pos
    i16 interpolate(std::vector<i16>::iterator pos, double dist);

    std::vector<i16> buffer;
    std::vector<i16>::iterator position;
    float sample_position;
    float ratio;
    std::vector<i16>::iterator next_sample;
    bool wrap;
};