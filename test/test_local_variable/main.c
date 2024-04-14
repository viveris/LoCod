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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#ifndef LOCOD_FPGA
#include "locod.h"
#endif

#define as_2dim_table_uchar(ptr, height) ((unsigned char (*)[height])ptr)
#define as_2dim_table_float(ptr, height) ((float (*)[height])ptr)
#define as_2dim_table_double(ptr, height) ((double (*)[height])ptr)


/****************************************************/
/***************** Accel functions ******************/
/****************************************************/
void test_func(float *param, float *result)
{
	float local_variable = 1234.56;

	*result = *param * local_variable;
}


/****************************************************/
/****************** Main function *******************/
/****************************************************/
#ifndef LOCOD_FPGA
int main(int argc, char *argv[])
{
	//Data for accelerator
	float param = 10.0;
	float res_fpga = 0.0;
	float res_cpu = 0.0;

	printf("Debut test local allocation\n");

	//----------------- Debut algo ---------------------
	init_locod(1);


	struct fpga_param acc_0_in = {0};
	acc_0_in.p = &param;
	acc_0_in.len = sizeof(param);

	struct fpga_param acc_0_out = {0};
	acc_0_out.p = &res_fpga;
	acc_0_out.len = sizeof(res_fpga);

	FPGA(test_func, acc_0_in, acc_0_out, 0);
	wait_accelerator(acc_0_out, 0);

	CPU(test_func, &param, &res_cpu);

	printf("Result FPGA = %f\n", res_fpga);
	printf("Result CPU = %f\n", res_cpu);


	deinit_locod();
	//------------------ Fin algo ----------------------

	return 0;
}
#endif