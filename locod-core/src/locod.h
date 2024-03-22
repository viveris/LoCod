#ifndef LOCOD_H
#define LOCOD_H


#if defined(TARGET_ultra96)
	#include "ultra96_defs.h"
#elif defined(TARGET_enclustra)
	#include "enclustra_defs.h"
#elif defined(TARGET_ngultra)
	#include "ngultra_defs.h"
#endif //TARGET


#define CPU(fct, param_ptr, result_ptr) do { fct(param_ptr, result_ptr); } while(0)

#define FPGA(fct, param_ptr, result_ptr, accel) do {\
	init_accelerator_memory(sizeof(*param_ptr), sizeof(*result_ptr), accel);\
	cp_param_and_result_to_accel_memory(param_ptr, result_ptr, accel);\
	start_accelerator(accel);\
} while(0)


int init_locod(int nb_acc);
int init_accelerator_memory(int param_len, int result_len, int accel);
int cp_param_and_result_to_accel_memory(void *param_addr, void *result_addr, int accel);
int cp_result_from_accel_memory(void *result_addr, int accel);
int start_accelerator(int accel);
int wait_accelerator(void *result_addr, int accel);
int get_time_ns_FPGA(int accel);
int deinit_locod(void);


#endif /* LOCOD_H */