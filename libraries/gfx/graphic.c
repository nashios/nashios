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

void gfx_graphic_draw(char *buffer, char *graphic, int scanline, int x, int y, int width, int height)
{
    for (int i = 0; i < height; i++)
    {
        char *i_buffer = buffer + (y + i) * scanline + x * 4;
        char *i_graphic = graphic + i * width * 4;

        for (int j = 0; j < width; j++)
        {
            *(uint32_t *)i_buffer = *(uint32_t *)i_graphic;
            i_buffer += 4;
            i_graphic += 4;
        }
    }
}

void gfx_graphic_alpha_draw(char *buffer, char *graphic, int scanline, int x, int y, int width, int height)
{
    for (int i = 0; i < height; i++)
    {
        char *i_buffer = buffer + (y + i) * scanline + x * 4;
        char *i_graphic = graphic + i * width * 4;

        for (int j = 0; j < width; j++)
        {
            gfx_graphic_set_pixel(i_buffer, i_graphic[0], i_graphic[1], i_graphic[2], i_graphic[3]);

            i_buffer += 4;
            i_graphic += 4;
        }
    }
}
