/**
 * @file libraries/c/termios.c
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
#include <sys/ioctl.h>
#include <termios.h>

int tcgetattr(int fildes, struct termios *termios_p) { return ioctl(fildes, TCGETS, termios_p); }