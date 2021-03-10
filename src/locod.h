#ifndef __LOCOD_H__
#define __LOCOD_H__

#include "map_memory.h"

struct param {
	void *p;
	size_t len;
};

#define CPU(fct, param, output) do { fct(param, output); } while(0)

#define FPGA(fct, param, output) do {\
	init_dma();\
	off_t phy_a = MEM_DMA_BASE ;\
	off_t phy_result = phy_a + param.len;\
	init_accelerator_memory(phy_a, 0, param.len);\
	cp_param_to_fpga(&param);\
	set_in_1(DMA_ADDR(phy_a));\
	set_out_1(DMA_ADDR(phy_result));\
	start_accelerator();\
	} while(0)

#endif /* __LOCOD_H__ */
