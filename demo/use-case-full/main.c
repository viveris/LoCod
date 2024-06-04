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
#define MALVAR_WIDTH		    1024
#define MALVAR_HEIGHT	        1024

#define ANSCOMBE_WIDTH          1020        
#define ANSCOMBE_HEIGHT         1020

#define RGB_TO_YUV_WIDTH        1020        
#define RGB_TO_YUV_HEIGHT       1020

#define REARANGE1_WIDTH		    1028
#define REARANGE1_HEIGHT	    1020

#define FILT1_REARANGE2_WIDTH   1020
#define FILT1_REARANGE2_HEIGHT	1020

#define REARANGE3_WIDTH		    1020
#define REARANGE3_HEIGHT	    1028

#define FILT2_REARANGE4_WIDTH   1020
#define FILT2_REARANGE4_HEIGHT	1020


/* ========== Accelerator structures to store input and output data ========== */
struct input_malvar {
    float bayer_img[MALVAR_HEIGHT][MALVAR_WIDTH];
    float G_at_RB[5][5];
    float R1[5][5];
    float R2[5][5];
    float R3[5][5];
    float B1[5][5];
    float B2[5][5];
    float B3[5][5];
};

struct output_malvar {
    float R[MALVAR_HEIGHT][MALVAR_WIDTH];
    float G[MALVAR_HEIGHT][MALVAR_WIDTH];
    float B[MALVAR_HEIGHT][MALVAR_WIDTH];
};

struct output_anscombe {
    float R[ANSCOMBE_HEIGHT][ANSCOMBE_WIDTH];
    float G[ANSCOMBE_HEIGHT][ANSCOMBE_WIDTH];
    float B[ANSCOMBE_HEIGHT][ANSCOMBE_WIDTH];
};

struct output_rgb_to_yuv {
    float YUV_1[RGB_TO_YUV_HEIGHT][RGB_TO_YUV_WIDTH];
    float YUV_2[RGB_TO_YUV_HEIGHT][RGB_TO_YUV_WIDTH];
    float YUV_3[RGB_TO_YUV_HEIGHT][RGB_TO_YUV_WIDTH];
};

struct output_rearange1 {
    float data[REARANGE1_HEIGHT][REARANGE1_WIDTH];
};

struct output_filt1_rearange2 {
    float data[FILT1_REARANGE2_HEIGHT][FILT1_REARANGE2_WIDTH];
};

struct output_rearange3 {
    float data[REARANGE3_HEIGHT][REARANGE3_WIDTH];
};

struct output_filt2_rearange4 {
    float data[FILT2_REARANGE4_HEIGHT][FILT2_REARANGE4_WIDTH];
};


/* ========== Accelerator functions ========== */
/* ------------------------------
 *            Malvar
 * ------------------------------
 */
float interpol_Malvar(	float matrix[MALVAR_HEIGHT][MALVAR_WIDTH], float kernel[5][5], unsigned int line_start, unsigned int col_start) {
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

void malvar(struct input_malvar *param, struct output_malvar *result) {
    for (unsigned int line=0; line<=(MALVAR_HEIGHT - 3); line = line + 2) {		/* Image line */
        for (unsigned int col=0; col<=(MALVAR_WIDTH - 3); col = col + 2) {		/* Image column */
            result->G[line][col+1] = param->bayer_img[line][col+1];
            result->G[line+1][col] = param->bayer_img[line+1][col];
            result->R[line][col] = param->bayer_img[line][col];
            result->B[line+1][col+1] = param->bayer_img[line+1][col+1];
            /* Allow to skip the edges of the image (2x2) */
            if (line >= 2 && line <= MALVAR_HEIGHT-3 && col >= 2 && col <= MALVAR_WIDTH-3) {
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

/* ------------------------------
 *            Anscombe
 * ------------------------------
 */
float anscombe_trf(float pixel, float ca, float cb)
{
    return (2/cb)*sqrt(cb*pixel + 3*pow(cb, 2)/8 + pow(ca, 2));
}

void anscombe(struct output_malvar *param, struct output_anscombe *result) {
    for (unsigned int line = 0; line < ANSCOMBE_HEIGHT; line++) {
        for (unsigned int col = 0; col < ANSCOMBE_WIDTH; col++) {
            result->R[line][col] = anscombe_trf(param->R[line+2][col+2], 0.1, 0.2);
            result->G[line][col] = anscombe_trf(param->G[line+2][col+2], 0.1, 0.2);
            result->B[line][col] = anscombe_trf(param->B[line+2][col+2], 0.1, 0.2);
        }
    }
}

/* ------------------------------
 *            RGB to YUV
 * ------------------------------
 */
void RGB_to_YUV(struct output_anscombe *param, struct output_rgb_to_yuv *result) {
    for (unsigned int line = 0; line < RGB_TO_YUV_HEIGHT; line++) {
        for (unsigned int col = 0; col < RGB_TO_YUV_WIDTH; col++) {
            result->YUV_1[line][col] = 0.299*param->R[line][col] + 0.587*param->G[line][col] + 0.114*param->B[line][col];
            result->YUV_2[line][col] = 128 - 0.168736*param->R[line][col] - 0.331264*param->G[line][col] + 0.5*param->B[line][col];
            result->YUV_3[line][col] = 128 + 0.5*param->R[line][col] - 0.418688*param->G[line][col] - 0.081312*param->B[line][col];
        }
    }
}

/* ------------------------------
 *            Filter
 * ------------------------------
 */
void rearange1(struct output_rgb_to_yuv *param, struct output_rearange1 *result) {
    for (unsigned int line = 0; line < RGB_TO_YUV_HEIGHT; line++) {
        result->data[line][0] = param->YUV_1[line][4];
        result->data[line][1] = param->YUV_1[line][3];
        result->data[line][2] = param->YUV_1[line][2];
        result->data[line][3] = param->YUV_1[line][1];

        result->data[line][RGB_TO_YUV_WIDTH+4] = param->YUV_1[line][RGB_TO_YUV_WIDTH-2];
        result->data[line][RGB_TO_YUV_WIDTH+5] = param->YUV_1[line][RGB_TO_YUV_WIDTH-3];
        result->data[line][RGB_TO_YUV_WIDTH+6] = param->YUV_1[line][RGB_TO_YUV_WIDTH-4];
        result->data[line][RGB_TO_YUV_WIDTH+7] = param->YUV_1[line][RGB_TO_YUV_WIDTH-5];
    }
    /* Fil in other cases */
    for (unsigned int line = 0; line < RGB_TO_YUV_HEIGHT; line++) {
        for (unsigned int col = 0; col < RGB_TO_YUV_WIDTH; col++) {
            result->data[line][col+4] = param->YUV_1[line][col];
        }
    }
}

void filt1_rearange2(struct output_rearange1 *param, struct output_filt1_rearange2 *result) {
    unsigned int ind = 0;
    float LP_Filter[9] = { 	 0.037828455507,
                          -0.023849465020,
                          -0.110624404418,
                           0.377402855613,
                           0.852698679009,
                           0.377402855613,
                          -0.110624404418,
                          -0.023849465020,
                           0.037828455507};

    float HP_Filter[7] = {	-0.064538882629,
                            0.040689417609,
                            0.418092273222,
                           -0.788485616406,
                            0.418092273222,
                            0.040689417609,
                           -0.064538882629};  

    for (unsigned int line = 0; line < FILT1_REARANGE2_HEIGHT; line++) {
        for (unsigned int col = 4; col < FILT1_REARANGE2_WIDTH+3; col += 2) {
            ind = (col+1-4)/2;
            result->data[line][ind] = 
                param->data[line][col-4]*LP_Filter[0] +
                param->data[line][col-3]*LP_Filter[1] +
                param->data[line][col-2]*LP_Filter[2] +
                param->data[line][col-1]*LP_Filter[3] +
                param->data[line][col+0]*LP_Filter[4] +
                param->data[line][col+1]*LP_Filter[5] +
                param->data[line][col+2]*LP_Filter[6] +
                param->data[line][col+3]*LP_Filter[7] +
                param->data[line][col+4]*LP_Filter[8];
            result->data[line][ind+(FILT1_REARANGE2_WIDTH/2)] =
                param->data[line][col+1-3]*HP_Filter[0] +
                param->data[line][col+1-2]*HP_Filter[1] +
                param->data[line][col+1-1]*HP_Filter[2] +
                param->data[line][col+1+0]*HP_Filter[3] +
                param->data[line][col+1+1]*HP_Filter[4] +
                param->data[line][col+1+2]*HP_Filter[5] +
                param->data[line][col+1+3]*HP_Filter[6];
        }
    }
}

void rearange3(struct output_filt1_rearange2 *param, struct output_rearange3 *result) {
    for (unsigned int col = 0; col < FILT1_REARANGE2_WIDTH; col++) {
        result->data[0][col] = param->data[4][col];
        result->data[1][col] = param->data[3][col];
        result->data[2][col] = param->data[2][col];
        result->data[3][col] = param->data[1][col];

        result->data[FILT1_REARANGE2_HEIGHT+8-4][col] = param->data[FILT1_REARANGE2_HEIGHT-2][col];
        result->data[FILT1_REARANGE2_HEIGHT+8-3][col] = param->data[FILT1_REARANGE2_HEIGHT-3][col];
        result->data[FILT1_REARANGE2_HEIGHT+8-2][col] = param->data[FILT1_REARANGE2_HEIGHT-4][col];
        result->data[FILT1_REARANGE2_HEIGHT+8-1][col] = param->data[FILT1_REARANGE2_HEIGHT-5][col];
    }
    /* Fil in other cases */
    for (unsigned int line = 0; line < FILT1_REARANGE2_HEIGHT; line++) {
        for (unsigned int col = 0; col < FILT1_REARANGE2_WIDTH; col++) {
            result->data[line+4][col] = param->data[line][col];
        }
    }
}

void filt2_rearange4(struct output_rearange3 *param, struct output_filt2_rearange4 *result) {
    unsigned int ind = 0;
    float LP_Filter[9] = {0.037828455507,
                          -0.023849465020,
                          -0.110624404418,
                           0.377402855613,
                           0.852698679009,
                           0.377402855613,
                          -0.110624404418,
                          -0.023849465020,
                           0.037828455507};
    float HP_Filter[7] = {-0.064538882629,
                            0.040689417609,
                            0.418092273222,
                           -0.788485616406,
                            0.418092273222,
                            0.040689417609,
                           -0.064538882629};

    for (unsigned int line = 4; line < FILT2_REARANGE4_HEIGHT+3; line += 2) {
        ind = (line+1-4)/2;
        for (unsigned int col = 0; col < FILT2_REARANGE4_WIDTH; col++) {
            result->data[ind][col] =
                param->data[line-4][col]*LP_Filter[0] +
                param->data[line-3][col]*LP_Filter[1] +
                param->data[line-2][col]*LP_Filter[2] +
                param->data[line-1][col]*LP_Filter[3] +
                param->data[line+0][col]*LP_Filter[4] +
                param->data[line+1][col]*LP_Filter[5] +
                param->data[line+2][col]*LP_Filter[6] +
                param->data[line+3][col]*LP_Filter[7] +
                param->data[line+4][col]*LP_Filter[8];
            result->data[ind+(FILT2_REARANGE4_HEIGHT/2)][col] =
                param->data[line+1-3][col]*HP_Filter[0] +
                param->data[line+1-2][col]*HP_Filter[1] +
                param->data[line+1-1][col]*HP_Filter[2] +
                param->data[line+1+0][col]*HP_Filter[3] +
                param->data[line+1+1][col]*HP_Filter[4] +
                param->data[line+1+2][col]*HP_Filter[5] +
                param->data[line+1+3][col]*HP_Filter[6];
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
    int ret;									                /* Return value */
    int opt;									                /* User option */
    struct timespec t_begin, t_end;	                            /* Clock timespecs */
    char *input_image_file;						                /* Input image file */
    char *output_malvar_file = "output_malvar_locod.bin";		/* Output Malvar file */
    char *output_filter_file = "output_filter_locod.bin";		/* Output Filter file */
    unsigned char *buffer;						                /* In and out buffer */
    struct input_malvar *input_malvar;			                /* Malvar input */
    struct output_malvar *output_malvar;	                    /* Malvar output */
    struct output_anscombe *output_anscombe;	                /* Anscombe output */
    struct output_rgb_to_yuv *output_rgb_to_yuv;                /* RGB to YUV output */
    struct output_rearange1 *output_rearange1;                  /* Rearange 1 output */
    struct output_filt1_rearange2 *output_filt1_rearange2;      /* Filt 1 Rearange 2 output */
    struct output_rearange3 *output_rearange3;                  /* Rearange 3 output */
    struct output_filt2_rearange4 *output_filt2_rearange4;      /* Filt 2 Rearange 4 output */
    float G_at_RB[5][5] = { {0,  0, -1, 0,  0},                 /* Malvar kernels */
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
                printf("Usage: %s [-i input_image_file]\n", argv[0]);
                return -1;
        }
    }

    /* Allocate structures and arrays */
    input_malvar = malloc(sizeof(struct input_malvar));
    output_malvar = malloc(sizeof(struct output_malvar));
    output_anscombe = malloc(sizeof(struct output_anscombe));
    output_rgb_to_yuv = malloc(sizeof(struct output_rgb_to_yuv));
    output_rearange1 = malloc(sizeof(struct output_rearange1));
    output_filt1_rearange2 = malloc(sizeof(struct output_filt1_rearange2));
    output_rearange3 = malloc(sizeof(struct output_rearange3));
    output_filt2_rearange4 = malloc(sizeof(struct output_filt2_rearange4));
    buffer = malloc(3*MALVAR_WIDTH*MALVAR_HEIGHT*sizeof(unsigned char));

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
    ret = load_byte_data(input_image_file, buffer, MALVAR_WIDTH*MALVAR_HEIGHT);
    if (ret != 0) {
        printf("Problem openning input image file\n");
        return ret;
    }

    /* Place raw data in accelerator input structure */
    for (unsigned int line=0; line<MALVAR_HEIGHT; line++) {						/* Image line */
        for (unsigned int col=0; col<MALVAR_WIDTH; col++) {						/* Image column */
            input_malvar->bayer_img[line][col] = (float)buffer[line*MALVAR_WIDTH + col];
        }
    }

    /* LoCod initialization */
    init_locod(6);

    /* Exuecute Malvar function in FPGA */
    FPGA(malvar, input_malvar, output_malvar, 0);
    wait_accelerator(output_malvar, 0);

    /* Execute Anscombe in CPU */
    CPU(anscombe, output_malvar, output_anscombe);

    /* Execute RGB to YUV in FPGA */
    FPGA(RGB_to_YUV, output_anscombe, output_rgb_to_yuv, 1);
    wait_accelerator(output_rgb_to_yuv, 1);

    /* Execute Rearange 1 in FPGA */
    /* From this step, the filtering is only applied on the YUV_1 layer */
    clock_gettime(CLOCK_MONOTONIC, &t_begin);
    FPGA(rearange1, output_rgb_to_yuv, output_rearange1, 2);
    wait_accelerator(output_rearange1, 2);

    /* Execute Filt 1 Rearange 2 in FPGA */
    FPGA(filt1_rearange2, output_rearange1, output_filt1_rearange2, 3);
    wait_accelerator(output_filt1_rearange2, 3);

    /* Execute Rearange 3 in FPGA */
    FPGA(rearange3, output_filt1_rearange2, output_rearange3, 4);
    wait_accelerator(output_rearange3, 4);

    /* Execute Filt 2 Rearange 4 in FPGA */
    FPGA(filt2_rearange4, output_rearange3, output_filt2_rearange4, 5);
    wait_accelerator(output_filt2_rearange4, 5);
    clock_gettime(CLOCK_MONOTONIC, &t_end);

    /* LoCod deinitialization */
    deinit_locod();

    /* Print YUV_1 filtered result */
    printf("YUV_1 filtered result :\n");
    print_image_TL((float *)output_filt2_rearange4->data, FILT2_REARANGE4_WIDTH);

    /* Print execution time */
    printf("FPGA CCSDS YUV1 execution time : %f sec\n", (double)(t_end.tv_nsec-t_begin.tv_nsec)/1000000000 + (double)(t_end.tv_sec-t_begin.tv_sec));

    /* Save Malvar output */
    /* Format [R0, G0, B0, R1, G1, B1, ... ] */
    for (unsigned int line=0; line<MALVAR_HEIGHT; line++) {						/* Image line */
        for (unsigned int col=0; col<MALVAR_WIDTH; col++) {						/* Image column */
            buffer[3*(line*MALVAR_WIDTH + col) + 0] = (unsigned char)output_malvar->R[line][col];
            buffer[3*(line*MALVAR_WIDTH + col) + 1] = (unsigned char)output_malvar->G[line][col];
            buffer[3*(line*MALVAR_WIDTH + col) + 2] = (unsigned char)output_malvar->B[line][col];
        }
    }
    ret = save_byte_data(output_malvar_file, buffer, 3*MALVAR_HEIGHT*MALVAR_WIDTH);
    if (ret != 0) {
        printf("Problem openning output malvar file\n");
        return ret;
    }

    /* Save final filtered YUV_1 output */
    for (unsigned int line=0; line<FILT2_REARANGE4_HEIGHT; line++) {						/* Image line */
        for (unsigned int col=0; col<FILT2_REARANGE4_WIDTH; col++) {						/* Image column */
            buffer[line*FILT2_REARANGE4_WIDTH + col] = (unsigned char)output_filt2_rearange4->data[line][col];
        }
    }
    ret = save_byte_data(output_filter_file, buffer, FILT2_REARANGE4_HEIGHT*FILT2_REARANGE4_WIDTH);
    if (ret != 0) {
        printf("Problem openning output image file\n");
        return ret;
    }

    /* Free memory */
    free(input_malvar);
    free(output_malvar);
    free(output_anscombe);
    free(output_rgb_to_yuv);
    free(output_rearange1);
    free(output_filt1_rearange2);
    free(output_rearange3);
    free(output_filt2_rearange4);
    free(buffer);

    return 0;
}
#endif /* LOCOD_FPGA */