/*                      __            ___             _ 
 *                     / /    ___    / __\  ___    __| |
 *                    / /    / _ \  / /    / _ \  / _` |
 *                   / /___ | (_) |/ /___ | (_) || (_| |
 *                   \____/  \___/ \____/  \___/  \__,_|
 *
 *             ***********************************************
 *                              LoCod Project
 *                  URL: https://github.com/viveris/LoCod
 *             ***********************************************
 *                  Copyright © 2024 Viveris Technologies
 *
 *                   Developed in partnership with CNES
 *               (DTN/TVO/ET: On-Board Data Handling Office)
 *
 *   This file is part of the LoCod framework.
 *
 *   The LoCod framework is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef LOCOD_FPGA
#include "locod.h"
#endif

struct param_long {
	int a;
};

struct param_test {
	int a;
	int b;
};

struct result_int {
	int a;
};

void acc_0(struct param_long *param,  struct result_int *result)
{
	int temp = 0;
	int i = 0;
	for(i; i<7500; i++){
	temp += (3 * (param->a));	
	result->a = temp;
	}
}

void acc_1(struct param_long *param, struct result_int *result)
{
	result->a = param->a * 10;
}
void acc_2(struct param_test *param, struct result_int *result)
{
	result->a = param->a + param->b;
}
void acc_3(struct param_test *param, struct result_int *result)
{
	result->a = param->a + param->b;
}
void acc_4(struct param_test *param, struct result_int *result)
{
	result->a = param->a + param->b;
}
void acc_5(struct param_test *param, struct result_int *result)
{
	result->a = param->a + param->b;
}
void acc_6(struct param_test *param, struct result_int *result)
{
	result->a = param->a + param->b;
}
void acc_7(struct param_test *param, struct result_int *result)
{
	result->a = param->a + param->b;
}

#ifndef LOCOD_FPGA
int main()
{
	struct result_int result[8] = { 0 };
	struct param_test param[8] = { 0 };
	struct param_long param_rep[2] = { 0 };
	int i;

	printf("LoCod multi_parallel example starting ...\n");

	init_locod(8);

	srand(time(0));
	param_rep[0].a = (rand() % 5)+1;
	fprintf(stdout, "Accelerator 0 data : %d\n", param_rep[0].a);
	for(i = 0; i<8; i++){
		param[i].a = rand() % 5;
		fprintf(stdout, "param a [%d] :%d\t", i, param[i].a);
		param[i].b = rand() % 15;
		fprintf(stdout, "param b [%d] :%d\n", i, param[i].b);
	}

	FPGA(acc_0, &param_rep[0], &result[0] , 0);
	cp_result_from_accel_memory(&result[0], 0);
	fprintf(stdout, "Accelerator 0 \"busy\" Result : %i\n", result[0].a);

	FPGA(acc_4, &param[4], &result[4], 4);
	wait_accelerator(&result[4], 4);
	fprintf(stdout, "Accelerator 4 Result : %i + %i = %i\n", param[4].a, param[4].b, result[4].a);

	cp_result_from_accel_memory(&result[0], 0);
	fprintf(stdout, "Accelerator 0 \"busy\" Result : %i\n", result[0].a);

	FPGA(acc_7, &param[7], &result[7], 7);
	wait_accelerator(&result[7], 7);							
	fprintf(stdout, "Accelerator 7 Result : %i + %i = %i\n", param[7].a, param[7].b, result[7].a);

	wait_accelerator(&result[0], 0);
	fprintf(stdout, "Accelerator 0 Result : %i\n", result[0].a);

	param_rep[1].a = result[0].a;

	FPGA(acc_1, &param_rep[1], &result[1], 1);
	wait_accelerator(&result[1], 1);
	fprintf(stdout, "Accelerator 1 Result :  %i * 10 = %i\n", param_rep[1].a, result[1].a);
	
	FPGA(acc_2, &param[2], &result[2], 2);
	FPGA(acc_6, &param[6], &result[6], 6);
	FPGA(acc_3, &param[3], &result[3], 3);
	FPGA(acc_5, &param[5], &result[5], 5);
	
	wait_accelerator(&result[2], 2);
	fprintf(stdout, "Accelerator 2 Result :  %i + %i = %i\n", param[2].a, param[2].b, result[2].a);
	wait_accelerator(&result[6], 6);
	fprintf(stdout, "Accelerator 6 Result :  %i + %i = %i\n", param[6].a, param[6].b, result[6].a);
	wait_accelerator(&result[3], 3);
	fprintf(stdout, "Accelerator 3 Result :  %i + %i = %i\n", param[3].a, param[3].b, result[3].a);
	wait_accelerator(&result[5], 5);
	fprintf(stdout, "Accelerator 5 Result :  %i + %i = %i\n", param[5].a, param[5].b, result[5].a);

	fprintf(stdout, "acc 0 time : %dns\n", get_time_ns_FPGA(0));

	deinit_locod();
		
	return 0;
}
#endif
