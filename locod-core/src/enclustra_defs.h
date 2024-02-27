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