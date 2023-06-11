#include <gfx/graphic.h>

void gfx_graphic_set_pixel(char *buffer, uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha)
{
    uint8_t red_color = buffer[0];
    uint8_t green_color = buffer[1];
    uint8_t blue_color = buffer[2];
    uint8_t alpha_color = buffer[3];

    float alpha_offset = alpha / (float)255;
    float alpha_raw = alpha_color / (float)255;
    float offset = (1 - alpha_offset) * alpha_raw;

    buffer[0] = red * alpha_offset + offset * red_color;
    buffer[1] = green * alpha_offset + offset * green_color;
    buffer[2] = blue * alpha_offset + offset * blue_color;
    buffer[3] = (alpha_offset + offset) * 255;
}

void gfx_graphic_draw(char *buffer, char *graphic, uint32_t scanline, int x, int y, uint32_t width, uint32_t height)
{
    for (uint32_t i = 0; i < height; i++)
    {
        char *i_buffer = buffer + (y + i) * scanline + x * 4;
        char *i_graphic = graphic + i * width * 4;

        for (uint32_t j = 0; j < width; j++)
        {
            *(uint32_t *)i_buffer = *(uint32_t *)i_graphic;
            i_buffer += 4;
            i_graphic += 4;
        }
    }
}

void gfx_graphic_alpha_draw(char *buffer, char *graphic, uint32_t scanline, int x, int y, uint32_t width,
                            uint32_t height)
{
    for (uint32_t i = 0; i < height; i++)
    {
        char *i_buffer = buffer + (y + i) * scanline + x * 4;
        char *i_graphic = graphic + i * width * 4;

        for (uint32_t j = 0; j < width; j++)
        {
            gfx_graphic_set_pixel(i_buffer, i_graphic[0], i_graphic[1], i_graphic[2], i_graphic[3]);

            i_buffer += 4;
            i_graphic += 4;
        }
    }
}
