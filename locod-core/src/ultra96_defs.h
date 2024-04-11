/*                      __            ___             _ 
 *                     / /    ___    / __\  ___    __| |
 *                    / /    / _ \  / /    / _ \  / _` |
 *                   / /___ | (_) |/ /___ | (_) || (_| |
 *                   \____/  \___/ \____/  \___/  \__,_|
 *
 *             ***********************************************
 *                              LoCod Project
 *                  URL: https://github.com/viveris/LoCod
 *             ***********************************************
 *                  Copyright © 2024 Viveris Technologies
 *
 *                   Developed in partnership with CNES
 *               (DTN/TVO/ET: On-Board Data Handling Office)
 *
 *   This file is part of the LoCod framework.
 *
 *   The LoCod framework is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <errno.h>



//Registers ADDR
#define REG_AXI_ADDR				0xA0000000

//Physical memory ADDR
#define DMA_BASE_ADDR 				0x40000000

#define FPGA_FREQ_HZ 				100000000

#define POLL_PERIOD_US         		1

#define DEBUG

#define LINUX