#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#ifndef LOCOD_FPGA
#include "locod.h"
#endif

#define as_2dim_table_uchar(ptr, height) ((unsigned char (*)[height])ptr)
#define as_2dim_table_float(ptr, height) ((float (*)[height])ptr)
#define as_2dim_table_double(ptr, height) ((double (*)[height])ptr)


/****************************************************/
/***************** Accel functions ******************/
/****************************************************/
struct param_test_s {
	float external_array[10];
};

struct result_test_s {
	float res_1;
	float res_2;
};

void test_func(struct param_test_s *param, struct result_test_s *result)
{
	float local_array[10] = {9, 8, 7, 6, 5, 4, 3, 2, 1, 0};

	result->res_1 = 0;
	result->res_2 = 0;

	for (int i = 0; i < 3; i++) {
		result->res_1 += param->external_array[i] * local_array[i];
	}

	for (int i = 0; i < 10; i++) {
		result->res_2 += param->external_array[i] * local_array[i];
	}
}


/****************************************************/
/****************** Main function *******************/
/****************************************************/
#ifndef LOCOD_FPGA
int main(int argc, char *argv[])
{
	//Data for accelerator
	struct param_test_s param_test = {0};
	struct result_test_s res_test_fpga = {0};
	struct result_test_s res_test_cpu = {0};

	for (int i = 0; i < 10; i++) {
		param_test.external_array[i] = i;
	}

	printf("Debut test\n");

	//----------------- Debut algo ---------------------
	init_accel_system(1);


	struct fpga_param acc_0_in = {0};
	acc_0_in.p = &param_test;
	acc_0_in.len = sizeof(param_test);

	struct fpga_param acc_0_out = {0};
	acc_0_out.p = &res_test_fpga;
	acc_0_out.len = sizeof(res_test_fpga);

	FPGA(test_func, acc_0_in, acc_0_out, 0);
	wait_accelerator(acc_0_out, 0);

	CPU(test_func, &param_test, &res_test_cpu);

	printf("Result FPGA : res_1=%f, res_2=%f\n", res_test_fpga.res_1, res_test_fpga.res_2);
	printf("Result CPU : res_1=%f, res_2=%f\n", res_test_cpu.res_1, res_test_cpu.res_2);


	deinit_accel_system();
	//------------------ Fin algo ----------------------

	return 0;
}
#endif