/**
 * @file kernel/system/sys.c
 * @author Saullo Bretas Silva (saullo.silva303@gmail.com)
 * @brief System controller
 * @version 0.1
 * @date 2022-05-24
 *
 * @copyright Copyright (C) 2022 Saullo Bretas Silva
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */
#include <kernel/stdio.h>
#include <kernel/system/sys.h>

void sys_panic(const char *message, const char *file, const char *function, int line, struct itr_registers *registers)
{
    printf("PANIC: Message = %s\n", message);
    printf("PANIC: File = %s\n", file);
    printf("PANIC: Function = %s\n", function);
    printf("PANIC: Line = %d\n", line);

    if (registers)
    {
        printf("PANIC: Registers:\n");
        printf("PANIC: * ES = 0x%04x, CS = 0x%04x\n", registers->es, registers->cs);
        printf("PANIC: * SS = 0x%04x, DS = 0x%04x\n", registers->ss, registers->ds);
        printf("PANIC: * FS = 0x%04x, GS = 0x%04x\n", registers->fs, registers->gs);
        printf("PANIC: * EAX = 0x%08x, EBX = 0x%08x\n", registers->eax, registers->ebx);
        printf("PANIC: * ECX = 0x%08x, EDX = 0x%08x\n", registers->ecx, registers->edx);
        printf("PANIC: * ESI = 0x%08x, EDI = 0x%08x\n", registers->esi, registers->edi);
        printf("PANIC: * EBP = 0x%08x, ESP = 0x%08x\n", registers->ebp, registers->esp);
        printf("PANIC: * EIP = 0x%08x, USERESP = 0x%08x\n", registers->eip, registers->useresp);
        printf("PANIC: * EFLAGS = 0x%08x, Error code = 0x%08x\n", registers->eflags, registers->err_code);
    }

    sys_stop();
}
