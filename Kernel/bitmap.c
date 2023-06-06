#include <Kernel/bitmap.h>

void bitmap_set(uint32_t *address, uint32_t bit) { address[bit / 32] |= (1 << (bit % 32)); }

void bitmap_unset(uint32_t *address, uint32_t bit) { address[bit / 32] &= ~(1 << (bit % 32)); }

bool bitmap_test(uint32_t *address, uint32_t bit) { return address[bit / 32] & (1 << (bit % 32)); }

uint32_t bitmap_first_free(uint32_t *address, size_t max)
{
    for (uint32_t i = 0; i < max / 32; i++)
    {
        if (address[i] == 0xFFFFFFFF)
            continue;

        for (uint32_t j = 0; j < 32; j++)
        {
            uint32_t bit = 1 << j;
            if (!(address[i] & bit))
                return i * 32 + j;
        }
    }

    return -1;
}

uint32_t bitmap_first_free_size(uint32_t *address, uint32_t max, size_t size)
{
    if (size == 0)
        return -1;

    if (size == 1)
        return bitmap_first_free(address, max);

    for (uint32_t i = 0; i < max / 32; i++)
    {
        if (address[i] == 0xFFFFFFFF)
            continue;

        for (uint32_t j = 0; j < 32; j++)
        {
            uint32_t bit = 1 << j;
            if (address[i] & bit)
                continue;

            uint32_t starting_bit = i * 32 + j;
            uint32_t free = 0;
            for (uint32_t count = 0; count <= size; count++)
            {
                if (!bitmap_test(address, starting_bit + count))
                    free++;

                if (free == size)
                    return i * 32 + j;
            }
        }
    }

    return -1;
}
