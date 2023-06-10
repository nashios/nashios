#pragma once

#include <stdint.h>

#define FBIOGET_VSCREENINFO 0x4600

struct fb_var_screeninfo
{
    uint32_t width;
    uint32_t height;
    uint32_t bits_per_pixel;
};
