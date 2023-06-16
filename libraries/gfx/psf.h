#pragma once

#include <stddef.h>
#include <stdint.h>

struct psf_font
{
    char *buffer;
    size_t size;
    uint16_t *unicode;
};

void psf_draw_line(struct psf_font *font, const char *string, int x, int y, uint32_t foreground, uint32_t background,
                   char *buffer, int scanline);
void psf_draw_char(struct psf_font *font, uint32_t ch, int x, int y, uint32_t foreground, uint32_t background,
                   char *buffer, int scanline);
struct psf_font *psf_open(const char *path);
