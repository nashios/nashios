#pragma once

#include <stdbool.h>
#include <stdint.h>

struct graphic
{
    int x;
    int y;
    int width;
    int height;
    bool transparent;
    char *buffer;
};

void gfx_graphic_set_pixel(char *buffer, uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha_raw);
void gfx_graphic_draw(char *buffer, char *graphic, uint32_t scanline, int x, int y, uint32_t width, uint32_t height);
void gfx_graphic_alpha_draw(char *buffer, char *graphic, uint32_t scanline, int x, int y, uint32_t width,
                            uint32_t height);
