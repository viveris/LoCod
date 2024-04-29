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
#include <string.h>
#include <math.h>
#include <time.h>
#include "locod.h"
#endif /* LOCOD_FPGA */


/* ========== Defines ========== */
#define IMAGE_WIDTH		1024
#define IMAGE_HEIGHT	1024
#define IMAGE_SIZE		IMAGE_WIDTH*IMAGE_HEIGHT


/* ========== Accelerator structures to store input and output data ========== */
struct input_malvar {
    float bayer_img[IMAGE_HEIGHT][IMAGE_WIDTH];
    float G_at_RB[5][5];
    float R1[5][5];
    float R2[5][5];
    float R3[5][5];
    float B1[5][5];
    float B2[5][5];
    float B3[5][5];
};

struct output_malvar {
    float R[IMAGE_HEIGHT][IMAGE_WIDTH];
    float G[IMAGE_HEIGHT][IMAGE_WIDTH];
    float B[IMAGE_HEIGHT][IMAGE_WIDTH];
};


/* ========== Accelerator functions ========== */
float interpol_Malvar(	float matrix[IMAGE_HEIGHT][IMAGE_WIDTH], float kernel[5][5], unsigned int line_start, unsigned int col_start) {
    float sum_col = 0;
    float result = 0;

    /* Sum of column members sum */
    for (int line=line_start; line<line_start+5; line++) {
        /* Sum of column members */
        for (int col=col_start; col<col_start+5; col++) {
            sum_col += matrix[line][col] * kernel[col-col_start][line-line_start];
        }
        result += sum_col;
        sum_col = 0;
    }

    /* No negative value saturation to 0 */
    if (result < 0) {
        return 0;
    } else {
        return result / 8;
    }
}

void acc_0(struct input_malvar *param, struct output_malvar *result) {
    for (unsigned int line=0; line<=(IMAGE_HEIGHT - 3); line = line + 2) {		/* Image line */
        for (unsigned int col=0; col<=(IMAGE_WIDTH - 3); col = col + 2) {		/* Image column */
            result->G[line][col+1] = param->bayer_img[line][col+1];
            result->G[line+1][col] = param->bayer_img[line+1][col];
            result->R[line][col] = param->bayer_img[line][col];
            result->B[line+1][col+1] = param->bayer_img[line+1][col+1];
            /* Allow to skip the edges of the image (2x2) */
            if (line >= 2 && line <= IMAGE_HEIGHT-3 && col >= 2 && col <= IMAGE_WIDTH-3) {
                /* Green channel */
                result->G[line][col] = interpol_Malvar(param->bayer_img, param->G_at_RB, line-2, col-2);
                result->G[line+1][col+1] = interpol_Malvar(param->bayer_img, param->G_at_RB, line-1, col-1);
                /* Red channel */
                result->R[line][col+1] = interpol_Malvar(param->bayer_img, param->R1, line-2, col-1);
                result->R[line+1][col] = interpol_Malvar(param->bayer_img, param->R2, line-1, col-2);
                result->R[line+1][col+1] = interpol_Malvar(param->bayer_img, param->R3, line-1, col-1);
                /* Blue channel */
                result->B[line][col] = interpol_Malvar(param->bayer_img, param->B3, line-2, col-2);
                result->B[line][col+1] = interpol_Malvar(param->bayer_img, param->B2, line-2, col-1);
                result->B[line+1][col] = interpol_Malvar(param->bayer_img, param->B1, line-1, col-2);
            }
        }
    }
}


/* ========== Utility functions ========== */
#ifndef LOCOD_FPGA
int load_byte_data(char *path, unsigned char *buffer, unsigned int size) {
    FILE *file;
    unsigned int n_read = 0;

    /* Open file */
    file = fopen(path,"rb");
    if (file == NULL) {
        printf("Cannot open file %s\n", path);
        return -1;
    }

    /* Load into memory and close file */
    n_read = fread(buffer, 1, size, file);
    if (n_read != size) {
        printf("Cannot read all data from file (number of bytes read %u)\n", n_read);
        fclose(file);
        return -1;
    }

    fclose(file);
    printf("File %s opened, read %u bytes\n", path, n_read);
    return 0;
}

int save_byte_data(char *path, unsigned char *buffer, unsigned int size) {
    FILE *file;
    unsigned int n_write = 0;

    /* Open file */
    file = fopen(path,"wb");
    if (file == NULL) {
        printf("Cannot open file %s\n", path);
        return -1;
    }

    /* Write and close file */
    n_write = fwrite(buffer, 1, size, file);
    if (n_write != size) {
        printf("Cannot write all data to file (number of bytes write %u)\n", n_write);
        fclose(file);
        return -1;
    }

    fclose(file);
    printf("File %s opened, write %u bytes\n", path, n_write);
    return 0;
}

void print_image_TL(float *image, unsigned int width) {
    for (int i=0; i<10; i++) {					/* Image line */
        for (int j=0; j<10; j++) {				/* Image column */
            printf("%f\t", ((float (*)[width])image)[i][j]);
        }
        printf("\n");
    }
}


/* ========== Main function ========== */
int main(int argc, char *argv[]) {
    /* Variables */
    int ret;													/* Return value */
    int opt;													/* User option */
    struct timespec begin_fpga, end_fpga, begin_cpu, end_cpu;	/* Clock timespecs */
    char *input_image_file;										/* Input image file */
    char *output_malvar_file = "output_malvar_locod.bin";		/* Output Malvar file */
    unsigned char *buffer;										/* In and out buffer */
    struct input_malvar *input_malvar;							/* Accelerator input */
    struct output_malvar *output_malvar_fpga;					/* Accelerator output FPGA */
    struct output_malvar *output_malvar_cpu;					/* Accelerator output CPU */
    float G_at_RB[5][5] = { {0,  0, -1, 0,  0},         		/* Malvar kernels */
                            {0,  0,  2, 0,  0,},
                            {-1, 2,  4, 2, -1},
                            {0,  0,  2, 0,  0},
                            {0,  0, -1, 0,  0,}
                          };

    float R1[5][5] = { {0,   0, -1,  0, 0},
                       {0,  -1,  4, -1, 0},
                       {0.5, 0,  5,  0, 0.5},
                       {0,  -1,  4, -1, 0},
                       {0,   0, -1,  0, 0}
                     };

    float R2[5][5] = { {0,  0, 0.5, 0, 0,},
                       {0, -1, 0,  -1, 0},
                       {-1, 4, 5,   4, -1},
                       {0, -1, 0,  -1, 0},
                       {0,  0, 0.5, 0, 0}
                     };

    float R3[5][5] = { {0,    0, -1.5, 0, 0},
                       {0,    2,  0,   2, 0},
                       {-1.5, 0,  6,   0, -1.5},
                       {0,    2,  0,   2, 0},
                       {0,    0, -1.5, 0, 0}
                     };

    float B1[5][5] = { {0,   0, -1,  0, 0},
                       {0,  -1,  4, -1, 0},
                       {0.5, 0,  5,  0, 0.5},
                       {0,  -1,  4, -1, 0},
                       {0,   0, -1,  0, 0}
                     };

    float B2[5][5] = { { 0,  0, 0.5, 0,  0},
                       { 0, -1, 0,  -1,  0},
                       {-1,  4, 5,   4, -1},
                       { 0, -1, 0,  -1,  0},
                       { 0,  0, 0.5, 0,  0}
                     };

    float B3[5][5] = { {0,    0, -1.5, 0,  0},
                       {0,    2,  0,   2,  0},
                       {-1.5, 0,  6,   0, -1.5},
                       {0,    2,  0,   2,  0},
                       {0,    0, -1.5, 0,  0}
                     };

    /* Begin */
    printf("CNES Image Processing Algorithm\n");

    /* Parsing user arguments */
    while ((opt = getopt(argc, argv, ":i:")) != -1) {
        switch (opt) {
            case 'i':
                input_image_file = optarg;
                break;
            case ':':
                printf("Option -%c requires an argument\n", optopt);
                return -1;
                break;
            case '?':
                printf("Unknown option -%c\n", optopt);
                return -1;
                break;
            default:
                printf("Usage: %s [-i input_file]\n", argv[0]);
                return -1;
        }
    }

    /* Allocate structures and arrays */
    input_malvar = malloc(sizeof(struct input_malvar));
    output_malvar_fpga = malloc(sizeof(struct output_malvar));
    output_malvar_cpu = malloc(sizeof(struct output_malvar));
    buffer = malloc(3*IMAGE_SIZE*sizeof(unsigned char));

    /* Fill in kernels in accelerator input structure */
    for (int i=0; i<5; i++) {
        for (int j=0; j<5; j++) {
            input_malvar->G_at_RB[i][j] = G_at_RB[i][j];
            input_malvar->R1[i][j] = R1[i][j];
            input_malvar->R2[i][j] = R2[i][j];
            input_malvar->R3[i][j] = R3[i][j];
            input_malvar->B1[i][j] = B1[i][j];
            input_malvar->B2[i][j] = B2[i][j];
            input_malvar->B3[i][j] = B3[i][j];
        }
    }

    /* Open image */
    ret = load_byte_data(input_image_file, buffer, IMAGE_SIZE);
    if (ret != 0) {
        printf("Problem openning input image file\n");
        return ret;
    }

    /* Place raw data in accelerator input structure */
    for (unsigned int line=0; line<IMAGE_HEIGHT; line++) {						/* Image line */
        for (unsigned int col=0; col<IMAGE_WIDTH; col++) {						/* Image column */
            input_malvar->bayer_img[line][col] = (float)buffer[line*IMAGE_WIDTH + col];
        }
    }

    /* LoCod initialization */
    init_locod(1);

    /* Exuecute acc_0 function in FPGA */
    clock_gettime(CLOCK_MONOTONIC, &begin_fpga);
    FPGA(acc_0, input_malvar, output_malvar_fpga, 0);
    wait_accelerator(output_malvar_fpga, 0);
    clock_gettime(CLOCK_MONOTONIC, &end_fpga);

    /* LoCod deinitialization */
    deinit_locod();

    /* Exuecute acc_0 function in CPU */
    clock_gettime(CLOCK_MONOTONIC, &begin_cpu);
    CPU(acc_0, input_malvar, output_malvar_cpu);
    clock_gettime(CLOCK_MONOTONIC, &end_cpu);

    /* Print image result FPGA */
    printf("R FPGA :\n");
    print_image_TL((float *)output_malvar_fpga->R, IMAGE_WIDTH);

    /* Print image result CPU */
    printf("R CPU :\n");
    print_image_TL((float *)output_malvar_cpu->R, IMAGE_WIDTH);

    /* Print execution time */
    printf("FPGA execution time : %f sec\nCPU execution time : %f sec\n",
        (double)(end_fpga.tv_nsec-begin_fpga.tv_nsec)/1000000000 + (double)(end_fpga.tv_sec-begin_fpga.tv_sec),
        (double)(end_cpu.tv_nsec-begin_cpu.tv_nsec)/1000000000 + (double)(end_cpu.tv_sec-begin_cpu.tv_sec));

    /* Place accelerator output into buffer : [R0, G0, B0, R1, G1, B1, ... ] */
    for (unsigned int line=0; line<IMAGE_HEIGHT; line++) {						/* Image line */
        for (unsigned int col=0; col<IMAGE_WIDTH; col++) {						/* Image column */
            buffer[3*(line*IMAGE_WIDTH + col) + 0] = (unsigned char)output_malvar_cpu->R[line][col];
            buffer[3*(line*IMAGE_WIDTH + col) + 1] = (unsigned char)output_malvar_cpu->G[line][col];
            buffer[3*(line*IMAGE_WIDTH + col) + 2] = (unsigned char)output_malvar_cpu->B[line][col];
        }
    }

    /* Write FPGA result into a file */
    ret = save_byte_data(output_malvar_file, buffer, 3*IMAGE_SIZE);
    if (ret != 0) {
        printf("Problem openning output image file\n");
        return ret;
    }

    /* Free memory */
    free(input_malvar);
    free(output_malvar_fpga);
    free(output_malvar_cpu);
    free(buffer);

    return 0;
}
#endif /* LOCOD_FPGA */