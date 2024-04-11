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
#include "demosaicing_float.h"
#endif


#define IMG_WIDTH 		1024
#define IMG_HEIGHT  	1024

#define as_2dim_table_uchar(ptr, height) ((unsigned char (*)[height])ptr)
#define as_2dim_table_float(ptr, height) ((float (*)[height])ptr)
#define as_2dim_table_double(ptr, height) ((double (*)[height])ptr)

//=============================== Function declarations =================================
void convert_array_uchar_to_float(unsigned char *input_array, float *output_array, unsigned int size);
void convert_array_float_to_uchar(float *input_array, unsigned char *output_array, unsigned int size);
void load_img(char *path, unsigned char *image, unsigned int size);
void save_img(char *path, unsigned char *image, unsigned int size);


/****************************************************/
/***************** Algo functions *******************/
/****************************************************/
float interpol_Malvar(float matrix[IMG_WIDTH*IMG_HEIGHT],
                           float kernel[5][5],
                           unsigned int line_start,
                           unsigned int col_start)
{
	float sum_col = 0;
	float result = 0;

	/* Sum of column members sum */
	for (int line=line_start; line<line_start+5; line++) {
		/* Sum of column members */
		for (int col=col_start; col<col_start+5; col++) {
			sum_col += as_2dim_table_float(matrix, IMG_HEIGHT)[col][line] * kernel[col-col_start][line-line_start];
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


/****************************************************/
/***************** Accel functions ******************/
/****************************************************/
struct param_malvar_s {
	float bayer_img[IMG_WIDTH*IMG_HEIGHT];
	float G_at_RB[5][5];
	float R1[5][5];
	float R2[5][5];
	float R3[5][5];
	float B1[5][5];
	float B2[5][5];
	float B3[5][5];
};

struct result_malvar_s {
	float R[IMG_WIDTH*IMG_HEIGHT];
	float G[IMG_WIDTH*IMG_HEIGHT];
	float B[IMG_WIDTH*IMG_HEIGHT];
};

void acc_0(struct param_malvar_s *param, struct result_malvar_s *result)
{
	for (unsigned int line=0; line<=(IMG_HEIGHT - 3); line = line + 2) {
		for (unsigned int col=0; col<=(IMG_WIDTH - 3); col = col + 2) {
			as_2dim_table_float(result->G, IMG_HEIGHT)[col+1][line] = as_2dim_table_float(param->bayer_img, IMG_HEIGHT)[col+1][line];
			as_2dim_table_float(result->G, IMG_HEIGHT)[col][line+1] = as_2dim_table_float(param->bayer_img, IMG_HEIGHT)[col][line+1];
			as_2dim_table_float(result->R, IMG_HEIGHT)[col][line] = as_2dim_table_float(param->bayer_img, IMG_HEIGHT)[col][line];
			as_2dim_table_float(result->B, IMG_HEIGHT)[col+1][line+1] = as_2dim_table_float(param->bayer_img, IMG_HEIGHT)[col+1][line+1];
			/* Allow to skip the edges of the image (2x2) */
			if (line >= 2 && line <= IMG_HEIGHT-3 && col >= 2 && col <= IMG_WIDTH-3) {
				/* Green channel */
				as_2dim_table_float(result->G, IMG_HEIGHT)[col][line] = interpol_Malvar(param->bayer_img, param->G_at_RB, line-2, col-2);
				as_2dim_table_float(result->G, IMG_HEIGHT)[col+1][line+1] = interpol_Malvar(param->bayer_img, param->G_at_RB, line-1, col-1);
				/* Red channel */
				as_2dim_table_float(result->R, IMG_HEIGHT)[col+1][line] = interpol_Malvar(param->bayer_img, param->R1, line-2, col-1);
				as_2dim_table_float(result->R, IMG_HEIGHT)[col][line+1] = interpol_Malvar(param->bayer_img, param->R2, line-1, col-2);
				as_2dim_table_float(result->R, IMG_HEIGHT)[col+1][line+1] = interpol_Malvar(param->bayer_img, param->R3, line-1, col-1);
				/* Blue channel */
				as_2dim_table_float(result->B, IMG_HEIGHT)[col][line] = interpol_Malvar(param->bayer_img, param->B3, line-2, col-2);
				as_2dim_table_float(result->B, IMG_HEIGHT)[col+1][line] = interpol_Malvar(param->bayer_img, param->B2, line-2, col-1);
				as_2dim_table_float(result->B, IMG_HEIGHT)[col][line+1] = interpol_Malvar(param->bayer_img, param->B1, line-1, col-2);
			}
		}
	}
}


/****************************************************/
/**************** Utility functions *****************/
/****************************************************/
void load_img(char *path, unsigned char *image, unsigned int size)
{
	FILE *ptr;
	int n_read = 0;

	/* Open file */
	ptr = fopen(path,"rb");
	if (ptr == NULL) {
		perror("Cannot open file");
	}

	/* Load into memory and close file */
	n_read = fread(image, 1, size, ptr);
	if (n_read != size) {
		printf("Cannot read all data from file (number of bytes read %d)\n", n_read);
	}
	fclose(ptr);

	fprintf(stdout, "File %s opened, size=%u bytes\n", path, size);
}

void save_img(char *path, unsigned char *image, unsigned int size)
{
	FILE *ptr;

	/* Open file */
	ptr = fopen(path,"wb");
	if (ptr == NULL) {
		perror("Cannot open file");
	}

	/* Write and close file */
	fwrite(image, 1, size, ptr);
	fclose(ptr);

	fprintf(stdout, "File %s opened, size=%u bytes\n", path, size);
}

void convert_array_uchar_to_float(unsigned char *input_array, float *output_array, unsigned int size)
{
	for (int i=0; i<size; i++) {
		output_array[i] = (float)input_array[i];
	}
}

void convert_array_float_to_uchar(float *input_array, unsigned char *output_array, unsigned int size)
{
	for (int i=0; i<size; i++) {
		output_array[i] = (unsigned char)round(input_array[i]);
	}
}


/****************************************************/
/****************** Main function *******************/
/****************************************************/
#ifndef LOCOD_FPGA
int main(int argc, char *argv[])
{
	unsigned char *imagette;

	//Data for accelerator 0
	struct param_malvar_s *param_malvar;
	struct result_malvar_s *result_malvar_fpga;
	struct result_malvar_s *result_malvar_cpu;

	param_malvar = malloc(sizeof(struct param_malvar_s));
	result_malvar_fpga = malloc(sizeof(struct result_malvar_s));
	result_malvar_cpu = malloc(sizeof(struct result_malvar_s));

	float G_at_RB[5][5] = { {0,  0, -1, 0,  0},
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


	printf("CNES Image Processing Algorithm\n");

	if (argc != 2)
	{
		printf("Mauvais arguments\n");
		return -1;
	}

	imagette = calloc(IMG_WIDTH*IMG_HEIGHT, sizeof(unsigned char));
	if (imagette == NULL)
	{
		printf("malloc error\n");
		return -1;
	}
	load_img(argv[1], imagette, IMG_WIDTH*IMG_HEIGHT);


	//----------------- Debut algo ---------------------
	init_locod(1);

	//Malvar
	convert_array_uchar_to_float(imagette, param_malvar->bayer_img, IMG_WIDTH*IMG_HEIGHT);

	for (int i=0; i<5; i++) {
		for (int j=0; j<5; j++) {
			param_malvar->G_at_RB[i][j] = G_at_RB[i][j];
			param_malvar->R1[i][j] = R1[i][j];
			param_malvar->R2[i][j] = R2[i][j];
			param_malvar->R3[i][j] = R3[i][j];
			param_malvar->B1[i][j] = B1[i][j];
			param_malvar->B2[i][j] = B2[i][j];
			param_malvar->B3[i][j] = B3[i][j];
		}
	}

	for (int i = 0 ; i < IMG_WIDTH*IMG_HEIGHT ; i++) {
		result_malvar_fpga->R[i] = 0.0;
		result_malvar_fpga->G[i] = 0.0;
		result_malvar_fpga->B[i] = 0.0;
		result_malvar_cpu->R[i] = 0.0;
		result_malvar_cpu->G[i] = 0.0;
		result_malvar_cpu->B[i] = 0.0;
	}

	FPGA(acc_0, param_malvar, result_malvar_fpga, 0);
	wait_accelerator(result_malvar_fpga, 0);

	CPU(acc_0, param_malvar, result_malvar_cpu);

	printf("R FPGA :\n");
	print_matrix_TL(result_malvar_fpga->R, IMG_HEIGHT);

	printf("R CPU :\n");
	print_matrix_TL(result_malvar_cpu->R, IMG_HEIGHT);

	printf("FPGA process time : %dns\n", get_time_ns_FPGA(0));

	deinit_locod();
	//------------------ Fin algo ----------------------


	return 0;
}
#endif