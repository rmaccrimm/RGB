#include "definitions.h"
#include <vector>

namespace sig
{
    void downsample(
        const std::vector<i16> &src, int src_rate, std::vector<i16> &dest, int dest_rate);  
};