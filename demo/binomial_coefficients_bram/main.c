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
#define MATRIX_MAX_SIZE		64



/* ========== Accelerator structures to store input and output data ========== */
struct param_acc0 {
    unsigned int nb_coeffs;
};

struct result_acc0 {
    unsigned int coefficients[MATRIX_MAX_SIZE];
};


/* ========== Accelerator functions ========== */
int pascal_triangle(unsigned int matrix[MATRIX_MAX_SIZE][MATRIX_MAX_SIZE], unsigned int nb_iter) {
    
    

    //initialisation of pascal matrix at 0 except first column
    for (unsigned int line=0; line<MATRIX_MAX_SIZE; line++) {
        for (unsigned int col=0; col<MATRIX_MAX_SIZE; col++) {
            if(col==0){
                matrix[line][col]=1;
            }
            else{
                matrix[line][col]=0;
            }
        }
    }
    if(nb_iter >= MATRIX_MAX_SIZE){
        nb_iter=MATRIX_MAX_SIZE-1;
    }

    unsigned int pascal_current_width = 1;
    for (unsigned int line=0; line<nb_iter; line++) {
        for (unsigned int col=0; col<pascal_current_width; col++) {
            matrix[line+1][col+1] = matrix[line][col] + matrix[line][col+1];
        }
        pascal_current_width++;
    }
    return 0;
  
}

void acc0(struct param_acc0 *param, struct result_acc0 *result) {
    
    unsigned int pascal_matrix[MATRIX_MAX_SIZE][MATRIX_MAX_SIZE];



    if(param->nb_coeffs==0){
            pascal_triangle(pascal_matrix, 0);
            for(int col=0; col<1;col++){
                result->coefficients[col]=pascal_matrix[1][col];
            }
        }    
    else{
        if(param->nb_coeffs<=MATRIX_MAX_SIZE){
        pascal_triangle(pascal_matrix, param->nb_coeffs-1); 
            for(int col=0; col<param->nb_coeffs;col++){
                result->coefficients[col]=pascal_matrix[param->nb_coeffs-1][col];
            }
        }        
        else{
            pascal_triangle(pascal_matrix, MATRIX_MAX_SIZE);
            for(int col=0; col<MATRIX_MAX_SIZE;col++){
                result->coefficients[col]=pascal_matrix[MATRIX_MAX_SIZE-1][col];
            }
        }
    }


    return;

}


#ifndef LOCOD_FPGA

/* ========== Main function ========== */
int main(int argc, char *argv[]) {
       //Variables
    struct param_acc0 param_acc_0 = {.nb_coeffs = 5};
    struct result_acc0 result_acc_0;

    //LoCod initialization
    init_locod(2);

    //Launching acc1 and acc2 function in the FPGA
    FPGA(acc0, &param_acc_0, &result_acc_0, 0);

    //Retreive outputs
    wait_accelerator(&result_acc_0, 0);

    //Print results
    printf("Acc 0 result = \n");
    for(int i=0;i<param_acc_0.nb_coeffs;i++){
        printf("\t%d", result_acc_0.coefficients[i]);
    }
    printf("\n");

    //Print execution time of the accelerator
    printf("Acc 0 execution time = %d ns\n", get_time_ns_FPGA(0));

    //LoCod de-initialization
    deinit_locod();

    return 0;
}
#endif /* LOCOD_FPGA */