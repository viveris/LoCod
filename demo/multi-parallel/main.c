#ifndef LOCOD_FPGA
#include "locod.h"
#endif

struct param_long {
	int a;
};

struct param_test {
	int a;
	int b;
};

void acc_0(struct param_long *param,  int *result)
{
	int temp = 0;
	int i = 0;
	for(i; i<7500; i++){
	temp += (3 * (param->a));	
	*result = temp;
	}
	
}

void acc_1(struct param_long *param, int *result)
{
	*result = param->a * 10;
}
void acc_2(struct param_test *param, int *result)
{
	*result = param->a + param->b;
}
void acc_3(struct param_test *param, int *result)
{
	*result = param->a + param->b;
}
void acc_4(struct param_test *param, int *result)
{
	*result = param->a + param->b;
}
void acc_5(struct param_test *param, int *result)
{
	*result = param->a + param->b;
}
void acc_6(struct param_test *param, int *result)
{
	*result = param->a + param->b;
}
void acc_7(struct param_test *param, int *result)
{
	*result = param->a + param->b;
}

#ifndef LOCOD_FPGA
int main()
{
	int result[8] = { 0 };
	struct param_test param[8] = { 0 };
	struct param_long param_rep[2] = { 0 };
	int i;

	printf("Locod multi_parallel example starting ...\n");

	init_locod(8);

	srand(time(0));
	param_rep[0].a = (rand() % 5)+1;
	fprintf(stdout, "Accelerator 0 data : %d\n", param_rep[0].a);
	for(i = 0; i<8; i++){
		param[i].a = rand() % 5;
		fprintf(stdout, "param a [%d] :%d\t", i, param[i].a);
		param[i].b = rand() % 15;
		fprintf(stdout, "param b [%d] :%d\n", i, param[i].b);
	}

	struct fpga_param param_a[8] = { 0 };
	for(i = 0; i<8; i++){
		param_a[i].p = &param[i];
		param_a[i].len = sizeof(struct param_test);
	}
	struct fpga_param param_b[2] = { 0 };
	for(i = 0; i<2; i++){	
		param_b[i].p = &param_rep[i];
		param_b[i].len = sizeof(struct param_long);
	}
	struct fpga_param param_result[8] = { 0 };
	for(i = 0; i<8; i++){
	param_result[i].p = &result[i];
	param_result[i].len = sizeof(int);
	}

	FPGA(acc_0, param_b[0], param_result[0] , 0);
	cp_result_from_accel_memory(param_result[0], 0);
	fprintf(stdout, "Accelerator 0 \"busy\" Result : %i\n", result[0]);

	FPGA(acc_4, param_a[4], param_result[4], 4);
	wait_accelerator(param_result[4], 4);
	fprintf(stdout, "Accelerator 4 Result : %i + %i = %i\n", param[4].a, param[4].b, result[4]);

	cp_result_from_accel_memory(param_result[0], 0);
	fprintf(stdout, "Accelerator 0 \"busy\" Result : %i\n", result[0]);

	FPGA(acc_7, param_a[7], param_result[7], 7);
	wait_accelerator(param_result[7], 7);							
	fprintf(stdout, "Accelerator 7 Result : %i + %i = %i\n", param[7].a, param[7].b, result[7]);

	wait_accelerator(param_result[0], 0);
	fprintf(stdout, "Accelerator 0 Result : %i\n", result[0]);

	param_rep[1].a = result[0];

	FPGA(acc_1, param_b[1], param_result[1], 1);
	wait_accelerator(param_result[1], 1);
	fprintf(stdout, "Accelerator 1 Result :  %i * 10 = %i\n", param_rep[1].a, result[1]);
	
	FPGA(acc_2, param_a[2], param_result[2], 2);
	FPGA(acc_6, param_a[6], param_result[6], 6);
	FPGA(acc_3, param_a[3], param_result[3], 3);
	FPGA(acc_5, param_a[5], param_result[5], 5);
	
	wait_accelerator(param_result[2], 2);
	fprintf(stdout, "Accelerator 2 Result :  %i + %i = %i\n", param[2].a, param[2].b, result[2]);
	wait_accelerator(param_result[6], 6);
	fprintf(stdout, "Accelerator 6 Result :  %i + %i = %i\n", param[6].a, param[6].b, result[6]);
	wait_accelerator(param_result[3], 3);
	fprintf(stdout, "Accelerator 3 Result :  %i + %i = %i\n", param[3].a, param[3].b, result[3]);
	wait_accelerator(param_result[5], 5);
	fprintf(stdout, "Accelerator 5 Result :  %i + %i = %i\n", param[5].a, param[5].b, result[5]);

	fprintf(stdout, "acc 0 time : %dns\n", get_time_ns_FPGA(0));

	deinit_locod();
		
	return 0;
}
#endif
