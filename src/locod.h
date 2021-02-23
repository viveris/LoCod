#ifndef __LOCOD_H__
#define __LOCOD_H__

#include <string.h>

#include "map_memory.h"
#include "time_measure.h"

struct param {
	void *p;
	size_t len;
};

#define CPU(fct, a, b, output) do { fct(a, b, output); } while(0)

#define FPGA(fct, a, b, output) do { \
	init_dma(); \
	void *mmap_ptr = NULL; \
	size_t mmap_len = a.len + b.len + output.len; \
	fprintf(stdout, "Try mapping 0x%X, size=%zu\n", MEM_DMA_BASE, mmap_len); \
	map_phys_addr(MEM_DMA_BASE, mmap_len, &mmap_ptr); \
	void *fpga_a = mmap_ptr; \
	off_t phy_a = MEM_DMA_BASE ; \
	void *fpga_b = fpga_a + a.len; \
	off_t phy_b = phy_a + a.len; \
	void *fpga_result = fpga_b + b.len; \
	off_t phy_result = phy_b + b.len; \
	struct timespec t1;\
	struct timespec t2;\
	clock_gettime(CLOCK_MONOTONIC, &t1);\
	memcpy(fpga_a, a.p, a.len); \
	memcpy(fpga_b, b.p, b.len); \
	clock_gettime(CLOCK_MONOTONIC, &t2);\
	struct timespec t_memcpy = diff_ts(&t1, &t2);\
	fprintf(stdout, "Copy parameters from CPU to FPGA: %zu bytes in %zu s and %zu ns\n",\
	        a.len + b.len, t_memcpy.tv_sec, t_memcpy.tv_nsec);\
	set_in_1(DMA_ADDR(phy_a)); \
	set_in_2(DMA_ADDR(phy_b)); \
	set_out_1(DMA_ADDR(phy_result)); \
	clock_gettime(CLOCK_MONOTONIC, &t1); \
	start_process(); \
	wait_process(); \
	clock_gettime(CLOCK_MONOTONIC, &t2);\
	struct timespec t_processing = diff_ts(&t1, &t2);\
	fprintf(stdout, "Processing time: %zu s and %zu ns\n",\
	        t_processing.tv_sec, t_processing.tv_nsec);\
	clock_gettime(CLOCK_MONOTONIC, &t1);\
	memcpy(output.p, fpga_result, output.len); \
	clock_gettime(CLOCK_MONOTONIC, &t2);\
	t_memcpy = diff_ts(&t1, &t2);\
	fprintf(stdout, "Copy result from FPGA to CPU: %zu bytes in %zu s and %zu ns\n",\
	        output.len, t_memcpy.tv_sec, t_memcpy.tv_nsec);\
	fprintf(stdout, "Processing time from FPGA: %u ns\n", get_time_ns_FPGA());\
	} while(0)

#endif /* __LOCOD_H__ */
