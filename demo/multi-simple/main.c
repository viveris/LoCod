#ifndef LOCOD_FPGA
#include "locod.h"
#endif

struct param_test {
	int a;
	int b;
};

void acc_1(struct param_test *param, int *result)
{
	*result = param->a + param->b;
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
void acc_8(struct param_test *param, int *result)
{
	*result = param->a + param->b;
}

#ifndef LOCOD_FPGA
int main()
{
	int result[8] = { 0 };
	struct param_test param[8] = { 0 };
	int i;
	srand(time(0));
	
	init_accel_system(8);

	for(i = 0; i<8; i++){
		param[i].a = rand() % 10;
		param[i].b = rand() % 15;
		fprintf(stdout, "A[%d] = %d\tB[%d] = %d\n",
	       i, param[i].a, i, param[i].b);
	}

	struct fpga_param param_a[8] = { 0 };
	for(i = 0; i<8; i++){
		param_a[i].p = &param[i];
		param_a[i].len = sizeof(struct param_test);
	}

	struct fpga_param param_result[8] = { 0 };
	for(i = 0; i<8; i++){
	param_result[i].p = &result[i];
	param_result[i].len = sizeof(int);
	}
	
	FPGA(acc_5, param_a[4], param_result[4], 4);
	wait_accelerator(param_result[4], 4);
	fprintf(stdout, "Accelerator 5 Result : %i + %i = %i\n", param[4].a, param[4].b, result[4]);
	FPGA(acc_2, param_a[1], param_result[1], 1);
	wait_accelerator(param_result[1], 1);
	fprintf(stdout, "Accelerator 2 Result : %i + %i = %i\n", param[1].a, param[1].b, result[1]);
	FPGA(acc_8, param_a[7], param_result[7], 7);
	wait_accelerator(param_result[7], 7);							
	fprintf(stdout, "Accelerator 8 Result : %i + %i = %i\n", param[7].a, param[7].b, result[7]);
	FPGA(acc_3, param_a[2], param_result[2], 2);
	wait_accelerator(param_result[2], 2);
	fprintf(stdout, "Accelerator 3 Result : %i + %i = %i\n", param[2].a, param[2].b, result[2]);
	FPGA(acc_7, param_a[6], param_result[6], 6);
	wait_accelerator(param_result[6], 6);
	fprintf(stdout, "Accelerator 7 Result : %i + %i = %i\n", param[6].a, param[6].b, result[6]);
	FPGA(acc_1, param_a[0], param_result[0], 0);
	wait_accelerator(param_result[0], 0);
	fprintf(stdout, "Accelerator 1 Result : %i + %i = %i\n", param[0].a, param[0].b, result[0]);
	FPGA(acc_4, param_a[3], param_result[3], 3);
	wait_accelerator(param_result[3], 3);
	fprintf(stdout, "Accelerator 4 Result : %i + %i = %i\n", param[3].a, param[3].b, result[3]);
	FPGA(acc_6, param_a[5], param_result[5], 5);
	wait_accelerator(param_result[5], 5);
	fprintf(stdout, "Accelerator 6 Result : %i + %i = %i\n", param[5].a, param[5].b, result[5]);

	deinit_accel_system();

	return 0;
}
#endif
