#ifndef LOCOD_FPGA
#include "locod.h"
#endif

void addition(float *a, float *b, float *result)
{
	*result = *a + *b;
}

void multiplication(float *a, float *b, float *result)
{
	*result = *a * *b;
}

#ifndef LOCOD_FPGA
int main(int argc, char **argv)
{
	float a = 16.25;
	float b = 3.34;
	float result = 0;

	struct fpga_param param_a = { 0 };
	param_a.p = &a;
	param_a.len = sizeof(float);

	struct fpga_param param_b = { 0 };
	param_b.p = &b;
	param_b.len = sizeof(float);

	struct fpga_param param_result = { 0 };
	param_result.p = &result;
	param_result.len = sizeof(float);

	FPGA(addition, param_a, param_b, param_result);
	fprintf(stdout, "Add result = %f\n", result);

	CPU(multiplication, &a, &b, &result);
	fprintf(stdout, "Mul result = %f\n", result);
}
#endif
