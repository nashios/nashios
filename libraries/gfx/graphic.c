#include <gfx/graphic.h>

void gfx_graphic_set_pixel(char *buffer, uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha)
{
    uint8_t raw_red = buffer[0];
    uint8_t raw_green = buffer[1];
    uint8_t raw_blue = buffer[2];
    uint8_t raw_alpha = buffer[3];

    float target_alpha = alpha / (float)255;
    float offset = (1 - target_alpha) * (raw_alpha / (float)255);
    buffer[0] = red * target_alpha + offset * raw_red;
    buffer[1] = green * target_alpha + offset * raw_green;
    buffer[2] = blue * target_alpha + offset * raw_blue;
    buffer[3] = (target_alpha + offset) * 255;
}
