#ifndef LOCOD_H
#define LOCOD_H

#include "map_memory.h"


#define CPU(fct, param, result) do { fct(param, result); } while(0)


#define FPGA(fct, param, result, accel) do {\
	init_accelerator_memory(param, result, accel);\
	cp_param_and_result_to_accel_memory(param, result, accel);\
	start_accelerator(accel);\
	} while(0)



/*
#define FPGA(fct, param, param_len, result, result_len, accel) do {\
	init_accelerator_memory(param_len, result_len, accel);\
	cp_param_to_accel_memory(param, param_len, accel);\
	start_accelerator(accel);\
	wait_accelerator(result, result_len, accel);\
	} while(0)

#define START_FPGA(fct, param, param_len, result_len, accel) do {\
	init_accelerator_memory(param_len, result_len, accel);\
	cp_param_to_accel_memory(param, param_len, accel);\
	start_accelerator(accel);\
	} while(0)

#define WAIT_FPGA(result, result_len, accel) do {\
	wait_accelerator(result, result_len, accel);\
	} while(0)
*/

#endif /* LOCOD_H */