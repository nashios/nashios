#pragma once

#include <stdbool.h>
#include <stdint.h>

#define ITR_CONTINUE false
#define ITR_STOP true

struct itr_registers
{
    uint32_t gs;
    uint32_t fs;
    uint32_t es;
    uint32_t ds;
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t unused;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;
    uint32_t number;
    uint32_t error_code;
    uint32_t eip;
    uint32_t cs;
    uint32_t eflags;
    uint32_t esp;
    uint32_t ss;
} __attribute__((packed));

typedef bool (*itr_handler_t)(struct itr_registers *registers);

void itr_init();
void itr_set_handler(int number, itr_handler_t handler);
void itr_unset_handler(int number, itr_handler_t handler);
void itr_handler(struct itr_registers *registers);
