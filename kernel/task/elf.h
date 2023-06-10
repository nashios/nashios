#pragma once

#include <stdint.h>

struct elf_layout
{
    uint32_t stack;
    uint32_t entry;
};

struct elf_layout *elf_load(const char *path);
void elf_unload();
