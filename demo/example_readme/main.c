#ifndef LOCOD_FPGA
#include "locod.h"
#endif //LOCOD_FPGA

#define SIZE 20

struct param_acc0 {
    int a;
    int b;
};

void acc0(struct param_acc0 *param, int *result) {
    *result = param->a * param->b;
}

struct param_acc1 {
    float a[SIZE];
};

struct result_acc1 {
    float a;
    float b;
};

void acc1(struct param_acc1 *param, struct result_acc1 *result) {
    int i = 0;
    result->a = 0.0;
    result->b = 0.0;

    for (i = 0; i < SIZE; i++) {
        result->a += param->a[i];
        result->b -= param->a[i];
    }
}

#ifndef LOCOD_FPGA
int main(void) {
    //Variables
    struct param_acc0 param_acc_0 = { .a = 3, .b = 7};
    int result_acc_0 = 0;
    struct param_acc1 param_acc_1;
    for (int i = 0; i < SIZE; i++) {
        param_acc_1.a[i] = i;
    }
    struct result_acc1 result_acc_1 = { .a = 0, .b = 0};

    //Locod initialization
    init_locod(2);

    //Launching acc1 and acc2 function in the FPGA
    FPGA(acc0, &param_acc_0, &result_acc_0, 0);
    FPGA(acc1, &param_acc_1, &result_acc_1, 1);

    //Retreive outputs
    wait_accelerator(&result_acc_0, 0);
    wait_accelerator(&result_acc_1, 1);

    //Print results
    printf("Acc 0 result : %d * %d = %d\n", param_acc_0.a, param_acc_0.b, result_acc_0);
    printf("Acc 1 result : sum of input values = %f, substraction of input values = %f\n", result_acc_1.a, result_acc_1.b);

    return 0;
} //End main()
#endif //LOCOD_FPGA