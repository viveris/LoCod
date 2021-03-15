#ifndef LOCOD_FPGA
#include "locod.h"
#endif

struct param_test {
	int a[2];
	int b[2];
};

void addition(struct param_test *param, int *result)
{
	result[0] = param->a[0] + param->b[0];
	result[1] = param->a[1] + param->b[1];
}

void multiplication(struct param_test *param, int *result)
{
	result[0] = param->a[0] * param->b[0];
	result[1] = param->a[1] * param->b[1];
}

#ifndef LOCOD_FPGA
int main(int argc, char **argv)
{
	int result[2] = { 0 };
	struct param_test param = { 0 };

	if (argc < 5) {
		param.a[0] = 2;
		param.a[1] = 8;
		param.b[0] = 3;
		param.b[1] = 5;
	} else {
		param.a[0] = atoi(argv[1]);
		param.a[1] = atoi(argv[2]);
		param.b[0] = atoi(argv[3]);
		param.b[1] = atoi(argv[4]);
	}

	fprintf(stdout, "A1 = %d  A2 = %d  B1 = %d  B2 = %d\n",
	        param.a[0], param.a[1], param.b[0], param.b[1]);

	struct fpga_param param_a = { 0 };
	param_a.p = &param;
	param_a.len = sizeof(struct param_test);

	struct fpga_param param_result = { 0 };
	param_result.p = result;
	param_result.len = 2*sizeof(int);

	FPGA(addition, param_a, param_result);
	wait_accelerator(&param_result);
	fprintf(stdout, "A1 + B1 = %d\n", result[0]);
	fprintf(stdout, "A2 + B2 = %d\n", result[1]);

	CPU(multiplication, &param, result);
	fprintf(stdout, "A1 x B1 = %d\n", result[0]);
	fprintf(stdout, "A2 x B2 = %d\n", result[1]);
}
#endif
