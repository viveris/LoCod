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

//GPIO offsets
#define GPIO_DATA   0
#define GPIO_TRI    1
#define GPIO2_DATA  2
#define GPIO2_TRI   3


//Connectors GPIO
#define arduino_a0_a5       1
#define arduino_ar0_ar13    2

//Pin modes
#define INPUT   1
#define OUTPUT  0



//Registers ADDR DDR
#define REG_AXI_ADDR				0x43C00000

//Registers ADDR GPIO
#define REG_AXI_ADDR_GPIO			0x81200000

//Physical memory ADDR
#define DMA_BASE_ADDR 				0x10000000

#define FPGA_FREQ_HZ 				100000000

#define POLL_PERIOD_US         		1

#define DEBUG

#define LINUX

#define GPIO