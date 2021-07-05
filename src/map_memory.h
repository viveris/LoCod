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

uint32_t get_reg_dur(int accel);

void init_accelerator_memory(off_t param_offset,
                             off_t result_offset,
							 off_t result_len,
							 int accel);
void set_mapped_param_offset(off_t off);
void set_mapped_result_offset(off_t off);
int cp_param_to_fpga(struct fpga_param *param, int accel);
int cp_result_from_fpga(struct fpga_param *result, int accel);
int wait_accelerator(struct fpga_param *result, int acccel);
int start_accelerator(int accel);
int init_dma();
int map_phys_addr(off_t phy_addr, size_t len, void **virt_ptr);
int unmap_phys_addr(void *virt_ptr, size_t len);
void print_ctrl_st_reg(void);
void dump_memory(void *ptr, size_t len);

#ifdef DEBUG
#define DEBUG_PRINT(...) do{ fprintf( stdout, __VA_ARGS__ ); } while( 0 )
#else
#define DEBUG_PRINT(...) do{ } while ( 0 )
#endif

#endif /* __MAP_MEMORY_H__ */
