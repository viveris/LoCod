#ifndef LOCOD_H
#define LOCOD_H


#if defined(TARGET_ULTRA96)
	#include "ultra96_defs.h"
#elif defined(TARGET_ENCLUSTRA)
	#include "enclustra_defs.h"
#elif defined(TARGET_NGULTRA)
	#include "ngultra_defs.h"
#endif //TARGET


#define CPU(fct, param, result) do { fct(param, result); } while(0)

#define FPGA(fct, param, result, accel) do {\
	init_accelerator_memory(param, result, accel);\
	cp_param_and_result_to_accel_memory(param, result, accel);\
	start_accelerator(accel);\
} while(0)


struct fpga_param {
	void *p;
	int len;
};


int init_locod(int nb_acc);
int init_accelerator_memory(struct fpga_param param, struct fpga_param result, int accel);
int cp_param_and_result_to_accel_memory(struct fpga_param param, struct fpga_param result, int accel);
int cp_result_from_accel_memory(struct fpga_param result, int accel);
int start_accelerator(int accel);
int wait_accelerator(struct fpga_param result, int accel);
int get_time_ns_FPGA(int accel);
int deinit_locod(void);


#endif /* LOCOD_H */