#include <kernel/math.h>
#include <kernel/memory/heap.h>
#include <kernel/memory/virtual.h>
#include <kernel/string.h>

#define HEAP_MIN_INDEX 8
#define HEAP_MAX_INDEX 32
#define HEAP_MAX_COUNT 16
#define HEAP_MAX_COMPLETED 5
#define HEAP_MAGIC 0xC001C0DE

struct heap_block
{
    uint32_t magic;
    uint32_t size;
    uint32_t real_size;
    int index;

    struct heap_block *split_left;
    struct heap_block *split_right;

    struct heap_block *next;
    struct heap_block *previous;
};

static struct heap_block *s_heap_free_pages[HEAP_MAX_INDEX] = {NULL};
static int s_heap_completed_pages[HEAP_MAX_INDEX] = {0};
static uint32_t s_heap_address = 0xD0000000;
static uint32_t s_heap_remaining = 0;

int heap_get_index(uint32_t size)
{
    if (size < (1 << HEAP_MIN_INDEX))
        return -1;

    int shift = HEAP_MIN_INDEX;
    while (shift < HEAP_MAX_INDEX)
    {
        if ((1 << shift) > (int)size)
            break;
        shift += 1;
    }

    return shift - 1;
}

void *heap_sbrk(size_t size)
{
    if (size == 0)
        return (char *)s_heap_address;

    char *base = (char *)s_heap_address;
    if (size <= s_heap_remaining)
        s_heap_remaining -= size;
    else
    {
        uint32_t request = DIV_ROUND_UP(size - s_heap_remaining, PAGE_SIZE);
        uint32_t physical = (uint32_t)physical_mm_allocate_size(request);
        uint32_t virtual = DIV_ROUND_UP(s_heap_address, PAGE_SIZE) * PAGE_SIZE;
        while (virtual < s_heap_address + size)
        {
            virtual_mm_map(g_virtual_directory, physical, virtual, PAGE_TBL_PRESENT | PAGE_TBL_WRITABLE);

            virtual += PAGE_SIZE;
            physical += PAGE_SIZE;
        }
        s_heap_remaining = virtual - (s_heap_address + size);
    }

    s_heap_address += size;
    memset(base, 0, size);

    return base;
}

struct heap_block *heap_allocate_block(uint32_t size)
{
    uint32_t usage = size + sizeof(struct heap_block);
    uint32_t pages = usage / PAGE_SIZE;
    if ((usage % PAGE_SIZE) != 0)
        pages += 1;

    if (pages < HEAP_MAX_COUNT)
        pages = HEAP_MAX_COUNT;

    struct heap_block *block = (struct heap_block *)heap_sbrk(pages * PAGE_SIZE);
    block->magic = HEAP_MAGIC;
    block->size = size;
    block->real_size = pages * PAGE_SIZE;
    block->index = -1;
    block->next = NULL;
    block->previous = NULL;
    block->split_right = NULL;
    block->split_left = NULL;

    return block;
}

void heap_insert_block(struct heap_block *block, int index)
{
    int real_index;
    if (index < 0)
    {
        real_index = heap_get_index(block->real_size - sizeof(struct heap_block));
        if (real_index < HEAP_MIN_INDEX)
            real_index = HEAP_MIN_INDEX;
    }
    else
        real_index = index;

    block->index = real_index;
    if (s_heap_free_pages[real_index] != NULL)
    {
        s_heap_free_pages[real_index]->previous = block;
        block->next = s_heap_free_pages[real_index];
    }
    s_heap_free_pages[real_index] = block;
}

void heap_remove_block(struct heap_block *block)
{
    if (s_heap_free_pages[block->index] == block)
        s_heap_free_pages[block->index] = block->next;

    if (block->previous != NULL)
        block->previous->next = block->next;

    if (block->next != NULL)
        block->next->previous = block->previous;

    block->next = NULL;
    block->previous = NULL;
    block->index = -1;
}

struct heap_block *heap_split_block(struct heap_block *block)
{
    uint32_t remainder = block->real_size - sizeof(struct heap_block) - block->size;
    struct heap_block *new_block = (struct heap_block *)((uint32_t)block + sizeof(struct heap_block) + block->size);
    new_block->magic = HEAP_MAGIC;
    new_block->real_size = remainder;
    new_block->next = NULL;
    new_block->previous = NULL;
    new_block->split_left = block;
    new_block->split_right = block->split_right;
    if (new_block->split_right != NULL)
        new_block->split_right->split_left = new_block;

    block->split_right = new_block;
    block->real_size -= new_block->real_size;

    heap_insert_block(new_block, -1);
    return new_block;
}

void *heap_malloc(size_t size)
{
    int index = heap_get_index(size);
    if (index < HEAP_MIN_INDEX)
        index = HEAP_MIN_INDEX;

    struct heap_block *block = s_heap_free_pages[index];
    while (block != NULL)
    {
        if ((block->real_size - sizeof(struct heap_block)) >= (size + sizeof(struct heap_block)))
            break;

        block = block->next;
    }

    if (block == NULL)
    {
        if ((block = heap_allocate_block(size)) == NULL)
            return NULL;

        index = heap_get_index(block->real_size - sizeof(struct heap_block));
    }
    else
    {
        heap_remove_block(block);

        if ((block->split_left == NULL) && (block->split_right == NULL))
            s_heap_completed_pages[index] -= 1;
    }

    block->size = size;
    uint32_t remainder = block->real_size - size - sizeof(struct heap_block) * 2;
    if ((int)(remainder) > 0)
    {
        int child_index = heap_get_index(remainder);
        if (child_index >= 0)
        {
            struct heap_block *new = heap_split_block(block);
            new = new;
        }
    }

    void *ptr = (void *)((uint32_t)block + sizeof(struct heap_block));
    return ptr;
}

void *heap_calloc(size_t nitems, size_t size)
{
    size_t target_size = nitems * size;
    void *target = heap_malloc(target_size);
    if (target)
        memset(target, 0x00, target_size);
    return target;
}

void *heap_realloc(void *ptr, size_t size)
{
    if (size == 0)
    {
        heap_free(ptr);
        return NULL;
    }

    if (ptr == NULL)
        return heap_malloc(size);

    struct heap_block *block = (struct heap_block *)((uint32_t)ptr - sizeof(struct heap_block));
    uint32_t target_size = block->size;
    if (target_size > size)
        target_size = size;

    void *target = heap_malloc(size);
    memcpy(target, ptr, target_size);
    heap_free(ptr);

    return target;
}

void heap_free(void *ptr)
{
    if (ptr == NULL)
        return;

    struct heap_block *block = (struct heap_block *)((uint32_t)ptr - sizeof(struct heap_block));
    if (block->magic != HEAP_MAGIC)
        return;

    while ((block->split_left != NULL) && (block->split_left->index >= 0))
    {
        struct heap_block *left = block->split_left;
        left->real_size += block->real_size;
        left->split_right = block->split_right;

        if (block->split_right != NULL)
            block->split_right->split_left = left;

        heap_remove_block(left);
    }

    while ((block->split_right != NULL) && (block->split_right->index >= 0))
    {
        struct heap_block *right = block->split_right;
        heap_remove_block(right);

        block->real_size += right->real_size;
        block->split_right = right->split_right;

        if (right->split_right != NULL)
            right->split_right->split_left = block;
    }

    int index = heap_get_index(block->real_size - sizeof(struct heap_block));
    if (index < HEAP_MIN_INDEX)
        index = HEAP_MIN_INDEX;

    if ((block->split_left == NULL) && (block->split_right == NULL))
    {
        if (s_heap_completed_pages[index] == HEAP_MAX_COMPLETED)
        {
            uint32_t pages = block->real_size / PAGE_SIZE;
            if ((block->real_size % PAGE_SIZE) != 0)
                pages += 1;

            if (pages < HEAP_MAX_COUNT)
                pages = HEAP_MAX_COUNT;

            return;
        }

        s_heap_completed_pages[index] += 1;
    }

    heap_insert_block(block, index);
}