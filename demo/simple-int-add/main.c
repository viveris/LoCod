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
int main(void)
{
	int result = 0;
	struct param_test param = { 0 };

	init_locod(1);

	param.a = 5;
	param.b = 7;

	printf("A = %d  B = %d\n", param.a, param.b);

	FPGA(acc_1, &param, &result, 0);
	wait_accelerator(&result, 0);
	printf("A + B = %d\n", result);

	CPU(acc_2, &param, &result);
	printf("A x B = %d\n", result);

	deinit_locod();

	return 0;
}
#endif
