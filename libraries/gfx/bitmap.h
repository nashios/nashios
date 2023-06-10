#pragma once

#include <gfx/graphic.h>

char *gfx_bitmap_open(const char *path);
void gfx_bitmap_draw(struct graphic *graphic, char *bitmap, int x, int y);
