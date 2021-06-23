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

#define ADDITION 1
#define MULTIPLICATION 2
struct fpga_process {
	unsigned int process;
	int *param_a;
	int *param_b;
	int *param_result;
};
void two_process_in_fpga(struct fpga_process *p, int *b, int *result)
{
	if (p->process == ADDITION) {
		addition(p->param_a, p->param_b, p->param_result);
	} else if (p->process == MULTIPLICATION) {
		multiplication(p->param_a, p->param_b, p->param_result);
	}
}

#ifndef LOCOD_FPGA
int main(int argc, char **argv)
{
	int a = 16;
	int b = 4;
	int result = 0;

	struct param param_result = { 0 };
	param_result.p = &result;
	param_result.len = sizeof(int);

	FPGA(addition, param_a, param_b, param_result);
	fprintf(stdout, "Add result = %d\n", result);

	CPU(multiplication, &a, &b, &result);
	fprintf(stdout, "Mul result = %d\n", result);
}
#endif
