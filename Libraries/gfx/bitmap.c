#include <fcntl.h>
#include <gfx/bitmap.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

#define BITMAP_RGB 0
#define BITMAP_BITFIELDS 3

struct bitmap_header
{
    int16_t signature;
    uint32_t file_size;
    uint32_t reserved;
    uint32_t offset;
} __attribute__((packed));

struct bitmap_core_header
{
    uint32_t header_size;
    int32_t width;
    int32_t height;
    uint16_t planes;
    uint16_t bits_p_pixel;
    uint32_t compression;
    uint32_t image_size;
};

void gfx_bitmap_bitfields_draw(struct graphic *graphic, struct bitmap_header *bitmap_header,
                               struct bitmap_core_header *core_header, int x, int y)
{
    uint32_t row_size = core_header->image_size / core_header->height;
    char *data = (char *)bitmap_header + bitmap_header->offset;
    uint8_t bytes_p_pixel = core_header->bits_p_pixel / 8;

    for (int i_y = core_header->height - 1; i_y >= 0; i_y--)
    {
        char *p_buffer = graphic->buffer + (y + core_header->height - i_y - 1) * graphic->width * 4 + x * 4;
        char *p_data = data + i_y * row_size;

        for (int i_x = 0; i_x < core_header->width; i_x++)
        {
            uint8_t alpha = bytes_p_pixel == 4 ? p_data[3] : 255;
            gfx_graphic_set_pixel(p_buffer, p_data[0], p_data[1], p_data[2], alpha);
            p_data += bytes_p_pixel;
            p_buffer += 4;
        }
    }
}

void gfx_bitmap_draw(struct graphic *graphic, char *bitmap, int x, int y)
{
    struct bitmap_header *bitmap_header = (struct bitmap_header *)bitmap;
    struct bitmap_core_header *core_header = (struct bitmap_core_header *)(bitmap + 0xE);

    switch (core_header->compression)
    {
    case BITMAP_RGB:
    case BITMAP_BITFIELDS:
        gfx_bitmap_bitfields_draw(graphic, bitmap_header, core_header, x, y);
        break;
    default:
        break;
    }
}

char *gfx_bitmap_open(const char *path)
{
    int fd = open(path, O_RDONLY);
    if (fd < 0)
        return NULL;

    struct stat stat = {};
    if (fstat(fd, &stat) < 0)
        return NULL;

    char *buffer = calloc(stat.st_size, sizeof(char));
    if (!buffer)
        return NULL;

    if (read(fd, buffer, stat.st_size) < 0)
        return NULL;

    if (close(fd) < 0)
        return NULL;

    return buffer;
}
