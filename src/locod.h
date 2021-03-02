#ifndef __LOCOD_H__
#define __LOCOD_H__

#include "map_memory.h"

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
	cp_param_to_fpga(fpga_a, &a, fpga_b, &b);\
	set_in_1(DMA_ADDR(phy_a)); \
	set_in_2(DMA_ADDR(phy_b)); \
	set_out_1(DMA_ADDR(phy_result)); \
	start_accelerator(); \
	cp_result_from_fpga(fpga_result, &output); \
	} while(0)

#endif /* __LOCOD_H__ */
