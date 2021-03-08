#ifndef LOCOD_FPGA
#include "locod.h"
#endif

struct param_test {
	float a;
	float b;
};

void addition(struct param_test *param, float *result)
{
	*result = param->a + param->b;
}

void multiplication(struct param_test *param, float *result)
{
	*result = param->a * param->b;
}

#ifndef LOCOD_FPGA
int main(int argc, char **argv)
{
	float result = 0;
	struct param_test param = { 0 };

	if (argc < 3) {
		param.a = 3.14;
		param.b = 12.8984;
	} else {
		param.a = atof(argv[1]);
		param.b = atof(argv[2]);
	}

	fprintf(stdout, "A = %f  B = %f\n", param.a, param.b);

	struct fpga_param param_a = { 0 };
	param_a.p = &param;
	param_a.len = sizeof(struct param_test);

	struct fpga_param param_result = { 0 };
	param_result.p = &result;
	param_result.len = sizeof(float);

	FPGA(addition, param_a, param_result);
	wait_accelerator();
	fprintf(stdout, "A + B = %f\n", result);

	CPU(multiplication, &param, &result);
	fprintf(stdout, "A x B = %f\n", result);
}
#endif
