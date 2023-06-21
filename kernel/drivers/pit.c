#include <kernel/arch/x86/cpu/io.h>
#include <kernel/cpu/processor.h>
#include <kernel/drivers/pit.h>
#include <kernel/interrupts/handler.h>
#include <kernel/lib/string.h>
#include <st/debug.h>

#define PIT_FREQUENCY 1193180
#define PIT_TICKS 1000

#define PIT_MASK 0xFF
#define PIT_RESYNC 1

#define PIT_CMOS_ADDRESS 0x70
#define PIT_CMOS_DATA 0x71
#define PIT_CMOS_SIZE 128

#define BCD_TO_DECIMAL(value) ((value / 16) * 10 + (value & 0xF))

enum pit_cmos
{
    PIT_CMOS_SECOND = 0,
    PIT_CMOS_MINUTE = 2,
    PIT_CMOS_HOUR = 4,
    PIT_CMOS_DAY = 7,
    PIT_CMOS_MONTH = 8,
    PIT_CMOS_YEAR = 9
};

uint32_t g_pit_ticks = 0;
uint32_t g_pit_subticks = 0;
static uint32_t s_pit_boot_time = 0;
static bool s_pit_behind = false;

uint8_t pit_cmos_is_updating()
{
    io_outb(PIT_CMOS_ADDRESS, 0x0A);
    return io_inb(PIT_CMOS_DATA) & 0x80;
}

void pit_dump_cmos(uint16_t *values)
{
    for (uint16_t index = 0; index < PIT_CMOS_SIZE; index++)
    {
        io_outb(PIT_CMOS_ADDRESS, index);
        values[index] = io_inb(PIT_CMOS_DATA);
    }
}

uint32_t pit_seconds_of_year(int years)
{
    uint32_t days = 0;
    years += 2000;

    while (years > 1969)
    {
        days += 365;
        if (years % 4 == 0)
        {
            if (years % 100 == 0)
            {
                if (years % 400 == 0)
                {
                    days++;
                }
            }
            else
            {
                days++;
            }
        }
        years--;
    }
    return days * 86400;
}

uint32_t pit_seconds_of_month(int months, int year)
{
    year += 2000;

    uint32_t days = 0;
    switch (months)
    {
    case 11:
        days += 30;
        // fall through
    case 10:
        days += 31;
        // fall through
    case 9:
        days += 30;
        // fall through
    case 8:
        days += 31;
        // fall through
    case 7:
        days += 31;
        // fall through
    case 6:
        days += 30;
        // fall through
    case 5:
        days += 31;
        // fall through
    case 4:
        days += 30;
        // fall through
    case 3:
        days += 31;
        // fall through
    case 2:
        days += 28;
        if ((year % 4 == 0) && ((year % 100 != 0) || (year % 400 == 0)))
        {
            days++;
        }
        // fall through
    case 1:
        days += 31;
        // fall through
    default:
        break;
    }

    return days * 86400;
}

uint32_t pit_read_cmos()
{
    while (pit_cmos_is_updating())
        ;

    uint16_t value[PIT_CMOS_SIZE] = {};
    pit_dump_cmos(value);

    uint16_t old_value[PIT_CMOS_SIZE] = {};
    do
    {
        memcpy(old_value, value, PIT_CMOS_SIZE);
        while (pit_cmos_is_updating())
            ;

        pit_dump_cmos(value);
    } while ((old_value[PIT_CMOS_SECOND] != value[PIT_CMOS_SECOND]) ||
             (old_value[PIT_CMOS_MINUTE] != value[PIT_CMOS_MINUTE]) ||
             (old_value[PIT_CMOS_HOUR] != value[PIT_CMOS_HOUR]) || (old_value[PIT_CMOS_DAY] != value[PIT_CMOS_DAY]) ||
             (old_value[PIT_CMOS_MONTH] != value[PIT_CMOS_MONTH]) ||
             (old_value[PIT_CMOS_YEAR] != value[PIT_CMOS_YEAR]));

    uint32_t years = pit_seconds_of_year(BCD_TO_DECIMAL(value[PIT_CMOS_YEAR]) - 1);
    uint32_t months =
        pit_seconds_of_month(BCD_TO_DECIMAL(value[PIT_CMOS_MONTH]) - 1, BCD_TO_DECIMAL(value[PIT_CMOS_YEAR]));
    uint32_t days = (BCD_TO_DECIMAL(value[PIT_CMOS_DAY]) - 1) * 86400;
    uint32_t hours = (BCD_TO_DECIMAL(value[PIT_CMOS_HOUR])) * 3600;
    uint32_t minutes = (BCD_TO_DECIMAL(value[PIT_CMOS_MINUTE])) * 60;
    uint32_t seconds = BCD_TO_DECIMAL(value[PIT_CMOS_SECOND]) + 0;

    return years + months + days + hours + minutes + seconds;
}

bool pit_handler(struct itr_registers *)
{
    if (++g_pit_subticks == PIT_TICKS || (s_pit_behind && ++g_pit_subticks == PIT_TICKS))
    {
        g_pit_ticks++;
        g_pit_subticks = 0;
        if (g_pit_ticks % PIT_RESYNC == 0)
        {
            uint32_t time = pit_read_cmos();
            uint32_t timer = time - s_pit_boot_time - g_pit_ticks;
            if (timer > 0)
                s_pit_behind = true;
            else
                s_pit_behind = false;
        }
    }

    return ITR_CONTINUE;
}

void pit_set_frequency(int frequency)
{
    int divisor = PIT_FREQUENCY / frequency;
    io_outb(0x43, 0x34);
    io_outb(0x40, divisor & PIT_MASK);
    io_outb(0x40, (divisor >> 8) & PIT_MASK);
}

void pit_init()
{
    s_pit_boot_time = pit_read_cmos();

    itr_set_handler(32, pit_handler);
    pit_set_frequency(PIT_TICKS);

    dbgln("PIT: Initialized");
}
