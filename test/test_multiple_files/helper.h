#ifndef HELPER_H
#define HELPER_H
struct param_test {
	int a[2];
	int b[2];
};

struct result_test {
	int a[2];
};

void acc_1(struct param_test *param, struct result_test *result);

void acc_2(struct param_test *param, struct result_test *result);

#endif