#ifndef LOCOD_FPGA
#include "locod.h"
#endif

void addition(int *a, int *b, int *result)
{
	result[0] = a[0] + b[0];
	result[1] = a[1] + b[1];
}

void multiplication(int *a, int *b, int *result)
{
	result[0] = a[0] * b[0];
	result[1] = a[1] * b[1];
}

#ifndef LOCOD_FPGA
int main(int argc, char **argv)
{
	int a[2];
	int b[2];
	int result[2] = { 0 };

	a[0] = 2;
	a[1] = 8;

	b[0] = 3;
	b[1] = 5;

	struct fpga_param param_a = { 0 };
	param_a.p = a;
	param_a.len = 2*sizeof(int);

	struct fpga_param param_b = { 0 };
	param_b.p = b;
	param_b.len = 2*sizeof(int);

	struct fpga_param param_result = { 0 };
	param_result.p = result;
	param_result.len = 2*sizeof(int);

	FPGA(addition, param_a, param_b, param_result);
	wait_accelerator();
	fprintf(stdout, "Add result0 = %d\n", result[0]);
	fprintf(stdout, "Add result1 = %d\n", result[1]);

	CPU(multiplication, a, b, result);
	fprintf(stdout, "Mul result0 = %d\n", result[0]);
	fprintf(stdout, "Mul result1 = %d\n", result[1]);
}
#endif
