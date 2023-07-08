/**
 * @file libraries/c/termios.h
 * @author Saullo Bretas Silva (saullo.silva55@outlook.com)
 * @brief
 * @version 0.0.1
 * @date 2023-07-08
 *
 * @copyright Nashi Operating System
 * Copyright (C) 2023 Saullo Bretas Silva <saullo.silva55@outlook.com>
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
#ifndef _TERMIOS_H
#define _TERMIOS_H 1

#include <stdint.h>

#define NCCS 32

#define ISIG 0x00001
#define ICANON 0x00002
#define XCASE 0x00004
#define ECHO 0x00008
#define ECHOE 0x00010
#define ECHOK 0x00020
#define ECHONL 0x00040
#define NOFLSH 0x00080
#define TOSTOP 0x00100
#define ECHOCTL 0x00200
#define ECHOPRT 0x00400
#define ECHOKE 0x00800
#define FLUSHO 0x01000
#define PENDIN 0x04000
#define IEXTEN 0x08000
#define EXTPROC 0x10000

#if __cplusplus
extern "C"
{
#endif

    typedef uint32_t tcflag_t;
    typedef uint8_t cc_t;
    typedef uint32_t speed_t;

    struct termios
    {
        tcflag_t c_iflag;
        tcflag_t c_oflag;
        tcflag_t c_cflag;
        tcflag_t c_lflag;
        cc_t c_cc[NCCS];
        speed_t c_ispeed;
        speed_t c_ospeed;
    };

    /**
     * @brief Get the parameters associated with the terminal
     *
     * @note See more at: https://pubs.opengroup.org/onlinepubs/9699919799/functions/tcgetattr.html
     *
     * @param fildes File descriptor associated with the terminal
     * @param termios_p Pointer to a termios structure
     * @return int 0 on success, -1 on failure
     */
    int tcgetattr(int fildes, struct termios *termios_p);

#if __cplusplus
}
#endif

#endif