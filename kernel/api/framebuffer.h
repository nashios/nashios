#pragma once

#include <stdint.h>

#define FBIOGET_VSCREENINFO 0x4600

struct fb_var_screeninfo
{
    uint32_t xres;
    uint32_t yres;
    uint32_t bits_per_pixel;
};
