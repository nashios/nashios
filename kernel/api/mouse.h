#pragma once

#include <stdint.h>

struct mouse_event
{
    int x;
    int y;
    int z;
    uint8_t buttons;
    uint32_t state;
};
