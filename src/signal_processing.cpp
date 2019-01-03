#include "signal_processing.h"

void sig::downsample(
    const std::vector<i16> &src, int src_rate, std::vector<i16> &dest, int dest_rate)
{
    double ratio = (double)src_rate / (double)dest_rate;
    auto src_it = src.begin();
    for (int i = 1; i < dest.size()/2 + 1; i++) {
        // interpolate between src[k] and src[k+1] -> dest[x]
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

        dest[2 * (i-1)] = h00*(double)src[k] + h10*m0 + h01*(double)src[k+1] + h11*m1;
        dest[2 * (i-1) + 1] = h00*(double)src[k] + h10*m0 + h01*(double)src[k+1] + h11*m1;
    }
}
