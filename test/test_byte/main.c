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
// test byte handling 

struct param_test {
	char arr [8]
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
	int i = 1;
	char result[8] = {0};
	struct param_test param = {{0}};
	init_locod(1);
	param.arr[0] = 'a';
	
	
	for(i;i < 8; i++){
		param.arr[i] = (param.arr[i-1] +1);
	}
	
	fprintf(stdout, "\nChar values\n");
	for(i = 0; i < 8; i++){
		fprintf(stdout, "'%c', ", param.arr[i]);
	}
	fprintf(stdout, "\n");
	fprintf(stdout, "Integer values\n");
	
	for(i = 0; i < 8; i++){
		fprintf(stdout, "%d, ", param.arr[i]);
	}
	fprintf(stdout, "\nAddresses\n");
	
	for(i = 0; i < 8; i++){
		fprintf(stdout, "%x, ", &param.arr[i]);
	}
	fprintf(stdout, "\n");
	
	struct fpga_param param_a = { 0 };
	param_a.p = &param;
	param_a.len = sizeof(struct param_test);

	struct fpga_param param_result = { 0 };
	param_result.p = &result;
	param_result.len = (8*sizeof(char));

	FPGA(acc_1, param_a, param_result, 0);
	wait_accelerator(param_result, 0);
	
	fprintf(stdout, "Char values in result\n");
	for(i = 0; i < 8; i++){
		fprintf(stdout, "'%c', ", result[i]);
	}
	fprintf(stdout, "\n");
	fprintf(stdout, "Integer values\n");
		for(i = 0; i < 8; i++){
			fprintf(stdout, "%d, ", result[i]);
		}
	fprintf(stdout, "\n");
	fprintf(stdout, "Addresses\n");
		for(i = 0; i < 8; i++){
			fprintf(stdout, "%x, ", &result[i]);
		}
	fprintf(stdout, "\n");
//	CPU(multiplication, &param, &result);
//	fprintf(stdout, "A x B = %f\n", result);

	deinit_locod();

	return 0;
}
#endif
