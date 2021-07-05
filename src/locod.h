#ifndef __LOCOD_H__
#define __LOCOD_H__

#include "map_memory.h"

struct param {
	void *p;
	size_t len;
};

#define CPU(fct, param, output) do { fct(param, output); } while(0)

 
#define FPGA(fct, param, output, accel) do {\
	init_accelerator_memory(0, param.len, output.len, accel);\
	cp_param_to_fpga(&param, accel);\
	start_accelerator(accel);\
	} while(0)

#endif /* __LOCOD_H__ */
