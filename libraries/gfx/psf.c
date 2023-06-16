#include <gfx/psf.h>
#include <limits.h>
#include <st/debug.h>
#include <st/math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#define PSF_MAGIC 0x864ab572
#define PSF_UNICODE_FLAG 0x01

struct psf_glyph
{
    uint32_t magic;
    uint32_t version;
    uint32_t headersize;
    uint32_t flags;
    uint32_t numglyph;
    uint32_t bytesperglyph;
    uint32_t height;
    uint32_t width;
};

void psf_draw_char(struct psf_font *font, uint32_t ch, int x, int y, uint32_t foreground, uint32_t background,
                   char *buffer, int scanline)
{
    if (!font || font->buffer == NULL)
        return;

    struct psf_glyph *glyphs = (struct psf_glyph *)font->buffer;
    if (glyphs->magic != PSF_MAGIC)
        return;

    int bytes_per_line = DIV_ROUND_UP(glyphs->width, 8);
    if (font->unicode != NULL)
        ch = font->unicode[ch];

    uint8_t *glyph = (uint8_t *)font->buffer + glyphs->headersize +
                     (ch > 0 && ch < glyphs->numglyph ? ch : 0) * glyphs->bytesperglyph;

    uint32_t offset = y * scanline + x * 4;
    uint32_t line = 0;
    uint32_t mask = 0;

    for (uint32_t i_y = 0; i_y < glyphs->height; i_y++)
    {
        line = offset;
        mask = 1 << (glyphs->width - 1);
        for (uint32_t i_x = 0; i_x < glyphs->width; i_x++)
        {
            if (((int)*glyph) & (mask))
                *((uint32_t *)(buffer + line)) = foreground;
            else if ((background & 0xFFFFFFF) != 0)
                *((uint32_t *)(buffer + line)) = background;

            mask >>= 1;
            line += 4;
        }
        glyph += bytes_per_line;
        offset += scanline;
    }
}

void psf_draw_line(struct psf_font *font, const char *string, int x, int y, uint32_t foreground, uint32_t background,
                   char *buffer, int scanline)
{
    if (!font || font->buffer == NULL)
        return;

    struct psf_glyph *glyphs = (struct psf_glyph *)font->buffer;
    if (glyphs->magic != PSF_MAGIC)
        return;

    size_t length = strlen(string);
    for (size_t i = 0; i < length; i++)
        psf_draw_char(font, string[i], x + i * glyphs->width, y, foreground, background, buffer, scanline);
}

int psf_parse(struct psf_font *font)
{
    if (!font || font->buffer == NULL)
        return -1;

    struct psf_glyph *glyphs = (struct psf_glyph *)font->buffer;
    if (glyphs->magic != PSF_MAGIC)
        return -1;

    char *glyphs_end = font->buffer + font->size;
    char *string = (char *)((char *)font->buffer + glyphs->headersize + glyphs->numglyph * glyphs->bytesperglyph);
    if (glyphs->flags & PSF_UNICODE_FLAG && string < glyphs_end)
    {
        uint16_t *unicode = (uint16_t *)calloc(USHRT_MAX, sizeof(uint16_t));
        if (!unicode)
            return -1;

        uint16_t glyph = 0;
        while (string < glyphs_end && glyph < USHRT_MAX)
        {
            uint16_t ch = (uint16_t)((uint8_t)string[0]);
            if (ch == 0xFF)
                glyph++;
            else
            {
                if ((ch & 128) != 0)
                {
                    if ((ch & 32) == 0)
                    {
                        ch = ((string[0] & 0x1F) << 6) + (string[1] & 0x3F);
                        string++;
                    }
                    else if ((ch & 16) == 0)
                    {
                        ch = ((((string[0] & 0xF) << 6) + (string[1] & 0x3F)) << 6) + (string[2] & 0x3F);
                        string += 2;
                    }
                    else if ((ch & 8) == 0)
                    {
                        ch = ((((((string[0] & 0x7) << 6) + (string[1] & 0x3F)) << 6) + (string[2] & 0x3F)) << 6) +
                             (string[3] & 0x3F);
                        string += 3;
                    }
                    else
                        ch = 0;
                }
                if (ch < USHRT_MAX)
                    unicode[ch] = glyph;
            }
            string++;
        }

        font->unicode = unicode;
    }

    return 0;
}

struct psf_font *psf_open(const char *path)
{
    int fd = open(path, O_RDONLY);
    if (fd < 0)
        return NULL;

    struct stat st = {};
    if (fstat(fd, &st) < 0)
        return NULL;

    char *buffer = (char *)calloc(st.st_size, sizeof(char));
    if (!buffer)
        return NULL;

    if (read(fd, buffer, st.st_size) < 0)
        return NULL;

    close(fd);

    struct psf_font *font = (struct psf_font *)calloc(1, sizeof(struct psf_font));
    if (!font)
        return NULL;

    font->buffer = buffer;
    font->size = st.st_size;

    if (psf_parse(font) < 0)
        return NULL;

    return font;
}
