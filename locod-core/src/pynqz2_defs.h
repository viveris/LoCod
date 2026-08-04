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

#define MAX_PINS    45 //for this board with the pins configured below

//Pins :
#define PIN_AR0     0
#define PIN_AR1     1
#define PIN_AR2     2
#define PIN_AR3     3
#define PIN_AR4     4
#define PIN_AR5     5
#define PIN_AR6     6
#define PIN_AR7     7
#define PIN_AR8     8
#define PIN_AR9     9
#define PIN_AR10    10
#define PIN_AR11    11
#define PIN_AR12    12
#define PIN_AR13    13

#define PIN_A0      14
#define PIN_A1      15
#define PIN_A2      16
#define PIN_A3      17
#define PIN_A4      18
#define PIN_A5      19

#define PIN_RPIO2   20
#define PIN_RPIO3   21
#define PIN_RPIO4   22
#define PIN_RPIO5   23
#define PIN_RPIO6   24
#define PIN_RPIO7   25
#define PIN_RPIO8   26
#define PIN_RPIO9   27
#define PIN_RPIO10  28
#define PIN_RPIO11  29
#define PIN_RPIO12  30
#define PIN_RPIO13  31
#define PIN_RPIO14  32
#define PIN_RPIO15  33
#define PIN_RPIO16  34
#define PIN_RPIO17  35
#define PIN_RPIO18  36
#define PIN_RPIO19  37
#define PIN_RPIO20  38
#define PIN_RPIO21  39
#define PIN_RPIO22  40
#define PIN_RPIO23  41
#define PIN_RPIO24  42
#define PIN_RPIO25  43
#define PIN_RPIO26  44



//Pin modes
#define INPUT   1
#define OUTPUT  0

//Pin states
#define LOW     0
#define HIGH    1

//Registers ADDR DDR
#define REG_AXI_ADDR				0x43C00000

//Registers ADDR GPIO
#define REG_AXI_ADDR_GPIO			0x41200000

//Physical memory ADDR
#define DMA_BASE_ADDR 				0x10000000

#define FPGA_FREQ_HZ 				100000000

#define POLL_PERIOD_US         		1

#define DEBUG

#define LINUX

#define GPIO