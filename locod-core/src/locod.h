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

#ifndef LOCOD_H
#define LOCOD_H


#if defined(TARGET_ultra96)
	#include "ultra96_defs.h"
#elif defined(TARGET_enclustra)
	#include "enclustra_defs.h"
#elif defined(TARGET_ngultra)
	#include "ngultra_defs.h"
#endif //TARGET


#define CPU(fct, param_ptr, result_ptr) do { fct(param_ptr, result_ptr); } while(0)

#define FPGA(fct, param_ptr, result_ptr, accel) do {\
	init_accelerator_memory(sizeof(*param_ptr), sizeof(*result_ptr), accel);\
	cp_param_and_result_to_accel_memory(param_ptr, result_ptr, accel);\
	start_accelerator(accel);\
} while(0)


int init_locod(int nb_acc);
int init_accelerator_memory(int param_len, int result_len, int accel);
int cp_param_and_result_to_accel_memory(void *param_addr, void *result_addr, int accel);
int cp_result_from_accel_memory(void *result_addr, int accel);
int start_accelerator(int accel);
int wait_accelerator(void *result_addr, int accel);
int get_time_ns_FPGA(int accel);
int deinit_locod(void);


#endif /* LOCOD_H */