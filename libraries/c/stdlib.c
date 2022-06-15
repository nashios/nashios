#include <stdlib.h>
#include <unistd.h>
#include <string.h>

static struct malloc_block *malloc_blocks = NULL;
static uint32_t malloc_address = 0;
static uint32_t malloc_remaining = 0;

struct malloc_block *heap_expand(struct malloc_block *last_block, size_t size)
{
    size_t request_size = size + sizeof(struct malloc_block);

    if (malloc_address == 0)
        malloc_address = (uint32_t)sbrk(0);

    void *p_heap = (void *)malloc_address;
    if (malloc_remaining <= request_size)
    {
        if (!sbrk(request_size))
            return NULL;

        malloc_remaining = (uint32_t)sbrk(0) - (malloc_address + request_size);
    }
    else
        malloc_remaining -= request_size;

    malloc_address += request_size;
    memset(p_heap, 0x0, request_size);

    struct malloc_block *block = p_heap;
    if (last_block)
        last_block->next = block;

    block->size = size;
    block->next = NULL;
    block->allocated = true;
    return block;
}

void *malloc(size_t size)
{
    if (size <= 0)
        return NULL;

    struct malloc_block *block;
    if (malloc_blocks)
    {
        struct malloc_block *last = malloc_blocks;
        for (block = malloc_blocks; block; block = block->next)
        {
            if (!block->allocated && block->size >= size)
                break;

            last = block;
        }

        if (!block)
            block = heap_expand(last, size);
        else
        {
            block->allocated = true;
            if (block->size > size + sizeof(struct malloc_block))
            {
                struct malloc_block *new_block = (struct malloc_block *)((char *)block + sizeof(struct malloc_block) + size);
                new_block->allocated = false;
                new_block->size = block->size - size - sizeof(struct malloc_block);
                new_block->next = block->next;

                block->size = size;
                block->next = new_block;
            }
        }
    }
    else
    {
        block = heap_expand(NULL, size);
        malloc_blocks = block;
    }

    if (!block)
        return NULL;

    return block + 1;
}

void *calloc(size_t nelem, size_t elsize)
{
    void *block = malloc(nelem * elsize);
    if (!block)
        return NULL;

    memset(block, 0x0, nelem * elsize);
    return block;
}

void free(void *ptr)
{
    if (!ptr)
        return;

    struct malloc_block *block = (struct malloc_block *)ptr - 1;
    block->allocated = false;
}

void exit(int status)
{
    _exit(status);
}

void _Exit(int status)
{
    exit(status);
}
