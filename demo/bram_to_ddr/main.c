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
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "locod.h"
#endif /* LOCOD_FPGA */


/* ========== Defines ========== */
#define MATRIX_SIZE		64



struct result_acc0 {
    float mem_ddr[MATRIX_SIZE];
};


static const unsigned int mem_bram[MATRIX_SIZE]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,};


void acc0(int *param, struct result_acc0 *result) {
    
    for(int i=0;i<MATRIX_SIZE;i++){
        result->mem_ddr[i]=mem_bram[i];
    }

    return;
}


#ifndef LOCOD_FPGA

/* ========== Main function ========== */
int main(int argc, char *argv[]) {
    //Variables
    int *param;
    struct result_acc0 result_acc_0;

    //LoCod initialization
    init_locod(1);

    //Launching acc0
    FPGA(acc0, &param, &result_acc_0, 0);

    //Retreive outputs
    wait_accelerator(&result_acc_0, 0);

    //Print results
    printf("Acc 0 result = \n");
    for(int i=0;i<MATRIX_SIZE;i++){
        printf("\t%f", result_acc_0.mem_ddr[i]);
    }
    printf("\n");

    //Print execution time of the accelerator
    printf("Acc 0 execution time = %d ns\n", get_time_ns_FPGA(0));

    //LoCod de-initialization
    deinit_locod();

    return 0;
}
#endif /* LOCOD_FPGA */