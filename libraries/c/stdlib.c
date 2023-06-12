#include <assert.h>
#include <st/pointer-arith.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/syscall.h>
#include <unistd.h>

#define MALLOC_MAGIC 0x12345678
#define MALLOC_MAX_SIZE 0x2000000

struct malloc_block
{
    size_t size;
    uint32_t magic;
    bool free;
    struct malloc_block *next;
};

static struct malloc_block *s_malloc_list = NULL;
static uint32_t s_malloc_address = 0;
static uint32_t s_malloc_remaining = 0;

void malloc_verify_block(struct malloc_block *block)
{
    assert(block);
    assert(block->magic == MALLOC_MAGIC);
    assert(block->size < MALLOC_MAX_SIZE);
}

void *malloc_sbrk(uint32_t size)
{
    if (!s_malloc_address)
        s_malloc_address = (uint32_t)sbrk(0);

    uint32_t address = s_malloc_address;
    if (size <= s_malloc_remaining)
        s_malloc_remaining -= size;
    else
    {
        sbrk(size);
        s_malloc_remaining = (uint32_t)sbrk(0) - (s_malloc_address + size);
    }

    s_malloc_address += size;
    return (void *)address;
}

struct malloc_block *malloc_request_space(struct malloc_block *last, size_t size)
{
    struct malloc_block *block = malloc_sbrk(size + sizeof(struct malloc_block));
    if (last)
        last->next = block;

    block->size = size;
    block->next = NULL;
    block->free = false;
    block->magic = MALLOC_MAGIC;
    return block;
}

void *malloc(size_t size)
{
    if (size <= 0)
        return NULL;
    size = ALIGN_UP(size, 4);

    struct malloc_block *block;
    if (s_malloc_list)
    {
        block = s_malloc_list;
        struct malloc_block *last = s_malloc_list;
        while (block && !(block->free && block->size >= size))
        {
            malloc_verify_block(block);
            last = block;
            block = block->next;
            if (block)
                malloc_verify_block(block);
        }

        if (block)
        {
            block->free = false;
            if (block->size > size + sizeof(struct malloc_block))
            {
                struct malloc_block *splited_block =
                    (struct malloc_block *)((char *)block + sizeof(struct malloc_block) + size);
                splited_block->free = true;
                splited_block->magic = MALLOC_MAGIC;
                splited_block->size = block->size - size - sizeof(struct malloc_block);
                splited_block->next = block->next;

                block->size = size;
                block->next = splited_block;
            }
        }
        else
            block = malloc_request_space(last, size);
    }
    else
    {
        block = malloc_request_space(NULL, size);
        s_malloc_list = block;
    }

    malloc_verify_block(block);
    return block ? (void *)(block + 1) : NULL;
}

void *calloc(size_t nitems, size_t size)
{
    void *block = malloc(nitems * size);
    if (block)
        memset(block, 0, nitems * size);
    return block;
}

void *realloc(void *ptr, size_t size)
{
    if (!ptr && size == 0)
    {
        free(ptr);
        return NULL;
    }
    else if (!ptr)
        return calloc(size, sizeof(char));

    void *newptr = calloc(size, sizeof(char));
    memcpy(newptr, ptr, size);
    return newptr;
}

void free(void *ptr)
{
    if (!ptr)
        return;

    struct malloc_block *block = (struct malloc_block *)ptr - 1;
    malloc_verify_block(block);
    block->free = true;
}

void exit(int status)
{
    syscall(__NR_exit_group, status);
    while (true)
        syscall(__NR_exit, status);
}

void abort(void)
{
    // FIXME: Implement abort() using SIGABRT
    exit(1);
}

int abs(int i) { return i < 0 ? -i : i; }
