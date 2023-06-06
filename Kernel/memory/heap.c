#include <Kernel/arch/i686/memory/virtual.h>
#include <Kernel/lib/string.h>
#include <Kernel/math.h>
#include <st/assert.h>
#include <stdbool.h>

#define HEAP_MAGIC 0xEF8E

struct heap_block
{
    size_t size;
    uint32_t magic;
    bool free;
    struct heap_block *next;
};

static struct heap_block *s_heap_list = NULL;
static uint32_t s_heap_address = 0xD0000000;
static uint32_t s_heap_remaining = 0;

void *heap_sbrk(size_t size)
{
    if (size == 0)
        return (char *)s_heap_address;

    char *p_heap = (char *)s_heap_address;
    if (size <= s_heap_remaining)
        s_heap_remaining -= size;
    else
    {
        uint32_t target_size = DIV_ROUND_UP(size - s_heap_remaining, PAGE_SIZE);
        uint32_t physical = (uint32_t)physical_mm_allocate_size(target_size);

        uint32_t virtual = DIV_ROUND_UP(s_heap_address, PAGE_SIZE) * PAGE_SIZE;

        while (virtual < s_heap_address + size)
        {
            virtual_mm_map(g_virtual_directory, physical, virtual, PAGE_TBL_PRESENT | PAGE_TBL_WRITABLE);
            physical += PAGE_SIZE;
            virtual += PAGE_SIZE;
        }

        s_heap_remaining = virtual - (s_heap_address + size);
    }

    s_heap_address += size;
    memset(p_heap, 0, size);
    return p_heap;
}

void heap_verify_block(struct heap_block *block)
{
    if (block->magic != HEAP_MAGIC || block->size > 0x2000000)
        assert_not_reached();
}

struct heap_block *heap_request_space(struct heap_block *last, size_t size)
{
    struct heap_block *block = heap_sbrk(size + sizeof(struct heap_block));
    if (last)
        last->next = block;

    block->size = size;
    block->next = NULL;
    block->free = false;
    block->magic = HEAP_MAGIC;
    return block;
}

void *heap_malloc(size_t size)
{
    if (size <= 0)
        return NULL;
    size = ALIGN_UP(size, 4);

    struct heap_block *block;
    if (s_heap_list)
    {
        block = s_heap_list;
        struct heap_block *last = s_heap_list;
        while (block && !(block->free && block->size >= size))
        {
            heap_verify_block(block);
            last = block;
            block = block->next;
            if (block)
                heap_verify_block(block);
        }

        if (block)
        {
            block->free = false;
            if (block->size > size + sizeof(struct heap_block))
            {
                struct heap_block *splited_block =
                    (struct heap_block *)((char *)block + sizeof(struct heap_block) + size);
                splited_block->free = true;
                splited_block->magic = HEAP_MAGIC;
                splited_block->size = block->size - size - sizeof(struct heap_block);
                splited_block->next = block->next;

                block->size = size;
                block->next = splited_block;
            }
        }
        else
            block = heap_request_space(last, size);
    }
    else
    {
        block = heap_request_space(NULL, size);
        s_heap_list = block;
    }

    heap_verify_block(block);

    if (block)
        return block + 1;
    else
        return NULL;
}

void *heap_calloc(size_t n, size_t size)
{
    void *block = heap_malloc(n * size);
    if (block)
        memset(block, 0, n * size);
    return block;
}

void heap_free(void *ptr)
{
    if (!ptr)
        return;

    struct heap_block *block = (struct heap_block *)ptr - 1;
    heap_verify_block(block);
    block->free = true;
}

void *heap_align(size_t size)
{
    uint32_t p_heap = (uint32_t)heap_sbrk(0);
    if (p_heap % size == 0)
        return NULL;

    uint32_t padding = DIV_ROUND_UP(p_heap, size) * size - p_heap;
    uint32_t required = sizeof(struct heap_block) * 2;
    while (padding <= 0xF0000000)
    {
        if (padding > required)
        {
            struct heap_block *last = s_heap_list;
            while (!last->next)
                last = last->next;
            struct heap_block *block = heap_request_space(last, padding - required);
            return block + 1;
        }

        padding += size;
    }
    return NULL;
}

void *heap_realloc(void *ptr, size_t size)
{
    if (!ptr && size == 0)
    {
        heap_free(ptr);
        return NULL;
    }
    else if (!ptr)
        return heap_calloc(size, sizeof(char));

    void *newptr = heap_calloc(size, sizeof(char));
    memcpy(newptr, ptr, size);
    return newptr;
}
