#ifndef MAP_MEMORY_H
#define MAP_MEMORY_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <errno.h>


struct fpga_param {
	void *p;
	size_t len;
};


int init_accel_system(int nb_acc);
int init_accelerator_memory(struct fpga_param param, struct fpga_param result, int accel);
int cp_param_and_result_to_accel_memory(struct fpga_param param, struct fpga_param result, int accel);
int cp_result_from_accel_memory(struct fpga_param result, int accel);
int start_accelerator(int accel);
int wait_accelerator(struct fpga_param result, int accel);
int get_time_ns_FPGA(int accel);
int deinit_accel_system();


#endif /* MAP_MEMORY_H */
