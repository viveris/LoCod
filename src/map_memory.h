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

/* Physical memory ADDR */
#define MEM_DMA_BASE 0x40000000

void set_in_1(uint32_t value);
void set_in_2(uint32_t value);
void set_out_1(uint32_t value);

int wait_process();
int start_process();
int init_dma();
int map_sync(void *virt_ptr, size_t len);
int map_phys_addr(off_t phy_addr, size_t len, void **virt_ptr);
int unmap_phys_addr(void *virt_ptr, size_t len);


#endif /* __MAP_MEMORY_H__ */
