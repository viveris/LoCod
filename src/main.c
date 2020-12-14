#ifndef LOCOD_FPGA
#include "locod.h"
#endif

void addition(int *a, int *b, int *result)
{
	*result = *a + *b;
}

void multiplication(int *a, int *b, int *result)
{
	*result = *a * *b;
}

#ifndef LOCOD_FPGA
int main(int argc, char **argv)
{
	int a = atoi(argv[1]);
	int b = atoi(argv[2]);
	int result = 0;

	struct param param_a = { 0 };
	param_a.p = &a;
	param_a.len = sizeof(int);

	struct param param_b = { 0 };
	param_b.p = &b;
	param_b.len = sizeof(int);

	struct param param_result = { 0 };
	param_result.p = &result;
	param_result.len = sizeof(int);

	FPGA(add, param_a, param_b, param_result);
	fprintf(stdout, "Add result = %d\n", result);

	CPU(multiplication, &a, &b, &result);
	fprintf(stdout, "Mul result = %d\n", result);
}
#endif
