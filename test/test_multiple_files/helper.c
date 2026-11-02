#include "helper.h"

void acc_1(struct param_test *param, struct result_test *result)
{
	result->a[0] = param->a[0] + param->b[0];
	result->a[1] = param->a[1] + param->b[1];
}

void acc_2(struct param_test *param, struct result_test *result)
{
	result->a[0] = param->a[0] * param->b[0];
	result->a[1] = param->a[1] * param->b[1];
}