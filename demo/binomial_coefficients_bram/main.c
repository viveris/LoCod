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
#include <time.h>
#include "locod.h"
#endif /* LOCOD_FPGA */


/* ========== Defines ========== */
#define MATRIX_MAX_SIZE		16



/* ========== Accelerator structures to store input and output data ========== */
struct param_acc0 {
    unsigned int nb_coeffs;
};

struct result_acc0 {
    unsigned int coefficients[MATRIX_MAX_SIZE];
    unsigned int matrix1[MATRIX_MAX_SIZE][MATRIX_MAX_SIZE];
    unsigned int matrix2[MATRIX_MAX_SIZE][MATRIX_MAX_SIZE];
};


//For FPGA Matrix need to by assigned precisely ! like this :
static unsigned int pascal_matrix[MATRIX_MAX_SIZE][MATRIX_MAX_SIZE]={

    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
    
};




/* ========== Accelerator functions ========== */
static inline int pascal_triangle(unsigned int matrix[MATRIX_MAX_SIZE][MATRIX_MAX_SIZE], unsigned int nb_iter) {
    
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
    for(int i=0;i<MATRIX_MAX_SIZE;i++){
        for(int j=0;j<MATRIX_MAX_SIZE;j++){
            result->matrix1[i][j]=pascal_matrix[i][j];
        }
    }
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

    for(int i=0;i<MATRIX_MAX_SIZE;i++){
        for(int j=0;j<MATRIX_MAX_SIZE;j++){
            result->matrix2[i][j]=pascal_matrix[i][j];
        }
    }
    return;
}


#ifndef LOCOD_FPGA

//For CPU, it's not mandatory and will work but if you assign it like this it'll not work with FPGA ! (it will be considered the first LINE full of 0 and not the first COLUMN !)
unsigned int pascal_matrix_cpu[MATRIX_MAX_SIZE][MATRIX_MAX_SIZE]={
    {1},
    {1},
    {1},
    {1},
    {1},
    {1},
    {1},
    {1},
    {1},
    {1},
    {1},
    {1},
    {1},
    {1},
    {1},
    {1}
};



/* ========== CPU functions ========== */

//Theses function are the same but don't use the same matrix definition !
int pascal_triangle_CPU(unsigned int matrix[MATRIX_MAX_SIZE][MATRIX_MAX_SIZE], unsigned int nb_iter) {
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

void cpu_pascal(struct param_acc0 *param, struct result_acc0 *result) {
    for(int i=0;i<MATRIX_MAX_SIZE;i++){
        for(int j=0;j<MATRIX_MAX_SIZE;j++){
            result->matrix1[i][j]=pascal_matrix_cpu[i][j];
        }
    }

    if(param->nb_coeffs==0){
            pascal_triangle_CPU(pascal_matrix_cpu, 0);
            for(int col=0; col<1;col++){
                result->coefficients[col]=pascal_matrix_cpu[1][col];
            }
        }    
    else{
        if(param->nb_coeffs<=MATRIX_MAX_SIZE){
        pascal_triangle_CPU(pascal_matrix_cpu, param->nb_coeffs-1); 
            for(int col=0; col<param->nb_coeffs;col++){
                result->coefficients[col]=pascal_matrix_cpu[param->nb_coeffs-1][col];
            }
        }        
        else{
            pascal_triangle_CPU(pascal_matrix_cpu, MATRIX_MAX_SIZE);
            for(int col=0; col<MATRIX_MAX_SIZE;col++){
                result->coefficients[col]=pascal_matrix_cpu[MATRIX_MAX_SIZE-1][col];
            }
        }
    }
    for(int i=0;i<MATRIX_MAX_SIZE;i++){
        for(int j=0;j<MATRIX_MAX_SIZE;j++){
            result->matrix2[i][j]=pascal_matrix_cpu[i][j];
        }
    }

    return;
}




/* ========== Main function ========== */
int main(int argc, char *argv[]) {
       //Variables
    struct param_acc0 param_acc_0 = {.nb_coeffs = 5};
    struct result_acc0 result_acc_0;

    struct param_acc0 param_cpu = {.nb_coeffs = 5};
    struct result_acc0 result_cpu;

    struct timespec begin_fpga, end_fpga, begin_cpu, end_cpu;	/* Clock timespecs */
    //LoCod initialization
    init_locod(1);

    //Launching acc0 function in the FPGA
    clock_gettime(CLOCK_MONOTONIC, &begin_fpga);
    FPGA(acc0, &param_acc_0, &result_acc_0, 0);
    wait_accelerator(&result_acc_0, 0);
    clock_gettime(CLOCK_MONOTONIC, &end_fpga);

    //LoCod de-initialization
    deinit_locod();


    /* Exuecute acc_0 function in CPU */
    clock_gettime(CLOCK_MONOTONIC, &begin_cpu);
    CPU(cpu_pascal, &param_cpu, &result_cpu);
    clock_gettime(CLOCK_MONOTONIC, &end_cpu);


    //Retreive outputs


    //Print results FPGA
    printf("FPGA Acc 0 result = \n");
    for(int i=0;i<param_acc_0.nb_coeffs;i++){
        printf("\t%d", result_acc_0.coefficients[i]);
    }
    printf("\n");
    printf("FPGA MATRIX 1\n");
    for(int i=0;i<MATRIX_MAX_SIZE;i++){
        for(int j=0;j<MATRIX_MAX_SIZE;j++){
            printf("%d\t", result_acc_0.matrix1[i][j]);
        }
        printf("\n");
    }
    printf("FPGA MATRIX 2\n");
    for(int i=0;i<MATRIX_MAX_SIZE;i++){
        for(int j=0;j<MATRIX_MAX_SIZE;j++){
            printf("%d\t", result_acc_0.matrix2[i][j]);
        }
        printf("\n");
    }



    //Print results CPU
    printf("CPU Acc 0 result = \n");
    for(int i=0;i<param_acc_0.nb_coeffs;i++){
        printf("\t%d", result_acc_0.coefficients[i]);
    }
    printf("\n");
    printf("CPU MATRIX 1\n");
    for(int i=0;i<MATRIX_MAX_SIZE;i++){
        for(int j=0;j<MATRIX_MAX_SIZE;j++){
            printf("%d\t", result_acc_0.matrix1[i][j]);
        }
        printf("\n");
    }
    printf("CPU MATRIX 2\n");
    for(int i=0;i<MATRIX_MAX_SIZE;i++){
        for(int j=0;j<MATRIX_MAX_SIZE;j++){
            printf("%d\t", result_acc_0.matrix2[i][j]);
        }
        printf("\n");
    }

    
    /* Print execution time */
    printf("FPGA execution time : %f sec\nCPU execution time : %f sec\n",
        (double)(end_fpga.tv_nsec-begin_fpga.tv_nsec)/1000000000 + (double)(end_fpga.tv_sec-begin_fpga.tv_sec),
        (double)(end_cpu.tv_nsec-begin_cpu.tv_nsec)/1000000000 + (double)(end_cpu.tv_sec-begin_cpu.tv_sec));



    return 0;
}
#endif /* LOCOD_FPGA */