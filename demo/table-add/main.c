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

struct param_test {
	int a[2];
	int b[2];
};

struct result_test {
	int a[2];
};

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

#ifndef LOCOD_FPGA
int main(int argc, char **argv)
{
	struct result_test result = { 0 };
	struct param_test param = { 0 };
	
	init_locod(1);

	if (argc < 5) {
		param.a[0] = 2;
		param.a[1] = 8;
		param.b[0] = 3;
		param.b[1] = 5;
	} else {
		param.a[0] = atoi(argv[1]);
		param.a[1] = atoi(argv[2]);
		param.b[0] = atoi(argv[3]);
		param.b[1] = atoi(argv[4]);
	}

	fprintf(stdout, "A1 = %d  A2 = %d  B1 = %d  B2 = %d\n",
	        param.a[0], param.a[1], param.b[0], param.b[1]);

	FPGA(acc_1, &param, &result, 0);
	wait_accelerator(&result, 0);
	fprintf(stdout, "A1 + B1 = %d\n", result.a[0]);
	fprintf(stdout, "A2 + B2 = %d\n", result.a[1]);

	CPU(acc_2, &param, &result);
	fprintf(stdout, "A1 x B1 = %d\n", result.a[0]);
	fprintf(stdout, "A2 x B2 = %d\n", result.a[1]);

	deinit_locod();

	return 0;
}
#endif
