#ifndef __MAP_MEMORY_H__
#define __MAP_MEMORY_H__

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/* Include for mmap */
#include <sys/mman.h>
/* Include for open files */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
/* Include for perror and errno handling */
#include <errno.h>
/* Inlcude for usleep */
#include <time.h>

#include "time_measure.h"

#define FPGA_FREQ_Hz 100000000 /* 100 Mhz */

/* Physical memory ADDR */
#define MEM_DMA_BASE 0x40000000

#define DMA_ADDR(phy_addr) (phy_addr - MEM_DMA_BASE)

struct fpga_param {
	void *p;
	size_t len;
};

void set_in_1(uint32_t value);
void set_in_2(uint32_t value);
void set_out_1(uint32_t value);
uint32_t get_reg_dur();

int cp_param_to_fpga(void *fpga_a, struct fpga_param *a,
                     void *fpga_b, struct fpga_param *b);
int cp_result_from_fpga(void *fpga_result, struct fpga_param *result);
int wait_accelerator();
int start_accelerator();
int init_dma();
int map_sync(void *virt_ptr, size_t len);
int map_phys_addr(off_t phy_addr, size_t len, void **virt_ptr);
int unmap_phys_addr(void *virt_ptr, size_t len);
void print_ctrl_st_reg(void);

#endif /* __MAP_MEMORY_H__ */
