#ifndef LOCOD_FPGA
#include "locod.h"
#endif
// test byte handling 

const int elements = 4;
struct param_test {
	char arr [4]
	};

void acc_1(struct param_test *param, char *result)
{
	int i =0;
	for(i; i < sizeof(param->arr); i++){
	result[i] = param->arr[i] + 1;
	}
}

#ifndef LOCOD_FPGA
int main(char **argv)
{	
	init_accel_system(1);
	int i = 1;
//	int j, k, l;
	char result[4] = {0};
	struct param_test param = {{0}};
	param.arr[0] = 'a';
	
	
	for(i;i < 4; i++){
		param.arr[i] = (param.arr[i-1] +1);
	}
	
	fprintf(stdout, "\nChar values\n");
	for(i = 0; i < 4; i++){
		fprintf(stdout, "'%c', ", param.arr[i]);
	}
	fprintf(stdout, "\n");
	fprintf(stdout, "Integer values\n");
	
	for(i = 0; i < 4; i++){
		fprintf(stdout, "%d, ", param.arr[i]);
	}
	fprintf(stdout, "\nAddresses\n");
	
	for(i = 0; i < 4; i++){
		fprintf(stdout, "%x, ", &param.arr[i]);
	}
	fprintf(stdout, "\n");
//	fprintf(stdout, "Addresses\n");
//	fprintf(stdout, "param[0] = %x  param[1] = %x ", &param.arr[0], &param.arr[1]);
//	fprintf(stdout, "param[2] = %x  param[3] = %x\n", &param.arr[2], &param.arr[3]);
	
	struct fpga_param param_a = { 0 };
	param_a.p = &param;
	param_a.len = sizeof(struct param_test);

	struct fpga_param param_result = { 0 };
	param_result.p = result;
	param_result.len = (4*sizeof(char));

	FPGA(acc_1, param_a, param_result, 0);
	wait_accelerator(param_result, 0);
//	fprintf(stdout, "A + B = %f\n", result);
	
	
	fprintf(stdout, "Char values in result\n");
	for(i = 0; i < 4; i++){
		fprintf(stdout, "'%c', ", result[i]);
	}
	fprintf(stdout, "\n");
	fprintf(stdout, "Integer values\n");
		for(i = 0; i < 4; i++){
			fprintf(stdout, "%d, ", result[i]);
		}
	fprintf(stdout, "\n");
	fprintf(stdout, "Addresses\n");
		for(i = 0; i < 4; i++){
			fprintf(stdout, "%x, ", &result[i]);
		}
	fprintf(stdout, "\n");
//	CPU(multiplication, &param, &result);
//	fprintf(stdout, "A x B = %f\n", result);

	deinit_accel_system();

	return 0;
}
#endif
