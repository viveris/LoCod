#include <stdio.h>
#include <stdlib.h>
#include "ngultra_init.h"


//Registers ADDR
#define REG_AXI_ADDR				0x10000000

//Physical memory ADDR
#define DMA_BASE_ADDR 				0x80000000

#define FPGA_FREQ_HZ 				25000000

#define POLL_PERIOD_US         		1

#define DEBUG

#define BAREMETAL