#include "signal_processing.h"

void sig::downsample(
    const std::vector<i16> &src, int src_rate, std::vector<i16> &dest, int dest_rate)
{
    double ratio = src_rate / dest_rate;
    auto src_it = src.begin();
    for (int i = 0; i < dest.size(); i++) {
        dest[i] = src[(int)(i*ratio + 0.5)];
    }
}
