#include "gpu.h"

GPU::GPU(u8 *mem): memory(mem) 
{
    framebuffer = new float[constants::screen_h * constants::screen_w];
}

GPU::~GPU()
{
    delete framebuffer;
}

void GPU::set_memory(u8 *mem) { memory = mem; }
