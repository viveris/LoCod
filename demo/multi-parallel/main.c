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


void acc_1(struct param_long *param,  int *result)
{
	int temp = 0;
	int i = 0;
	for(i; i<7500; i++){
	temp += (3 * (param->a));	
	*result = temp;
	}
	
}

void acc_2(struct param_long *param, int *result)
{
	*result = param->a * 10;
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
	struct param_long param_rep[2] = { 0 };
	int i;
	srand(time(0));
	init_dma();
	param_rep[0].a = (rand() % 5)+1;
	fprintf(stdout, "Accelerator 1 data : %d\n", param_rep[0].a);
	for(i = 0; i<8; i++){
		param[i].a = rand() % 5;
		fprintf(stdout, "param a [%d] data : %d\n", i, param[i].a);
		param[i].b = rand() % 15;
		fprintf(stdout, "param b [%d] data : %d\n", i, param[i].b);
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

	FPGA(acc_1, param_b[0], param_result[0] , 0);
	cp_result_from_fpga(&param_result[0], 0);
	fprintf(stdout, "Accelerator 1 \"busy\" Result : %d\n", result[0]);
	FPGA(acc_5, param_a[4], param_result[4], 4);
	
	wait_accelerator(&param_result[4], 4);
	fprintf(stdout, "Accelerator 5 Result : %d\n", result[4]);
	cp_result_from_fpga(&param_result[0], 0);
	fprintf(stdout, "Accelerator 1 \"busy\" Result : %d\n", result[0]);
	FPGA(acc_8, param_a[7], param_result[7], 7);
	wait_accelerator(&param_result[7], 7);							
	fprintf(stdout, "Accelerator 8 Result : %d\n", result[7]);

	wait_accelerator(&param_result[0], 0);
	fprintf(stdout, "Accelerator 1 Result : %d\n", result[0]);
	param_rep[1].a = result[0];
	FPGA(acc_2, param_b[1], param_result[1], 1);
	wait_accelerator(&param_result[1], 1);
	fprintf(stdout, "Accelerator 2 Result : %d\n", result[1]);
	
	FPGA(acc_3, param_a[2], param_result[2], 2);
	FPGA(acc_7, param_a[6], param_result[6], 6);
	FPGA(acc_4, param_a[3], param_result[3], 3);
	FPGA(acc_6, param_a[5], param_result[5], 5);
	
	
	
	
	
	
	

	

	wait_accelerator(&param_result[2], 2);
	fprintf(stdout, "Accelerator 3 Result : %d\n", result[2]);
	

	wait_accelerator(&param_result[6], 6);
	fprintf(stdout, "Accelerator 7 Result : %d\n", result[6]);
	

	wait_accelerator(&param_result[3], 3);
	fprintf(stdout, "Accelerator 4 Result : %d\n", result[3]);
	
	wait_accelerator(&param_result[5], 5);
	fprintf(stdout, "Accelerator 6 Result : %d\n", result[5]);

		
	
/*	CPU(addition, &param, result);
	fprintf(stdout, "A1 + B1 = %d\n", result[0]);
	fprintf(stdout, "A2 + B2 = %d\n", result[1]);*/
}
#endif
