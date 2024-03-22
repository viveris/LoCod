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
	*result = param->a * param->b;
}

#ifndef LOCOD_FPGA
int main(int argc, char **argv)
{
	int result = 0;
	struct param_test param = { 0 };

	init_locod(1);

	if (argc < 3) {
		param.a = 5;
		param.b = 7;
	} else {
		param.a = atoi(argv[1]);
		param.b = atoi(argv[2]);
	}

	fprintf(stdout, "A = %d  B = %d\n", param.a, param.b);

	FPGA(acc_2, &param, &result, 0);
	wait_accelerator(&result, 0);
	fprintf(stdout, "A x B = %d\n", result);

	CPU(acc_1, &param, &result);
	fprintf(stdout, "A + B = %d\n", result);

	deinit_locod();

	return 0;
}
#endif
