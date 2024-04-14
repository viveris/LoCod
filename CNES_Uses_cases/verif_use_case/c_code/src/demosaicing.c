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

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <math.h>
#include <string.h>
#include <time.h>

#define as_2dim_table_uchar(ptr, height) ((unsigned char (*)[height])ptr)
#define as_2dim_table_float(ptr, height) ((float (*)[height])ptr)
#define as_2dim_table_double(ptr, height) ((double (*)[height])ptr)

//=============================== Function declarations =================================
void print_matrix_TL(double *matrix, unsigned int height);
void print_matrix_TR(double *matrix, unsigned int height);
void print_matrix_BL(double *matrix, unsigned int height);
void print_matrix_BR(double *matrix, unsigned int height);
void convert_array_double_to_uchar(double *input_array, unsigned char *output_array, unsigned int size);
void load_img(char *path, unsigned char *image, unsigned int size);
void save_img(char *path, unsigned char *image, unsigned int size);


/****************************************************/
/***************** Algo functions *******************/
/****************************************************/
double interpolation_Malvar(unsigned char *matrix,
                           float kernel[5][5],
                           unsigned int line_start,
                           unsigned int col_start)
{
	double sum_col = 0;
	double result = 0;

	/* Sum of column members sum */
	for (int line=line_start; line<line_start+5; line++) {
		/* Sum of column members */
		for (int col=col_start; col<col_start+5; col++) {
			sum_col += as_2dim_table_uchar(matrix, 1024)[col][line] * kernel[col-col_start][line-line_start];
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

double anscombe_trf(double pixel, float ca, float cb)
{
	return (2/cb)*sqrt(cb*pixel + 3*pow(cb, 2)/8 + pow(ca, 2));
}

double *RGB_to_YUV(double R,
                   double G,
                   double B,
                   double *YUV_1,
                   double *YUV_2,
                   double *YUV_3)
{
	*YUV_1 = 0.299*R + 0.587*G + 0.114*B;
	*YUV_2 = 128 - 0.168736*R - 0.331264*G + 0.5*B;
	*YUV_3 = 128 + 0.5*R - 0.418688*G - 0.081312*B;
}

void DWT_CCSDS_single_level(double *chan_YUV, unsigned int height, unsigned int width)
{
	double LP_Filter[9] = {0.037828455507,
	                      -0.023849465020,
	                      -0.110624404418,
	                       0.377402855613,
	                       0.852698679009,
	                       0.377402855613,
	                      -0.110624404418,
	                      -0.023849465020,
	                       0.037828455507};

	double HP_Filter[7] = {-0.064538882629,
	                        0.040689417609,
	                        0.418092273222,
	                       -0.788485616406,
	                        0.418092273222,
	                        0.040689417609,
	                       -0.064538882629};

	double LP_InvFilter[7] = {-0.064538882629,
	                          -0.040689417609,
	                           0.418092273222,
	                           0.788485616406,
	                           0.418092273222,
	                          -0.040689417609,
	                          -0.064538882629};

	double HP_InvFilter[9] = {-0.037828455507,
	                          -0.023849465020,
	                           0.110624404418,
	                           0.377402855613,
	                          -0.852698679009,
	                           0.377402855613,
	                           0.110624404418,
	                          -0.023849465020,
	                          -0.037828455507};

	double *X_sym_col = calloc(height*(width+8), sizeof(double));
	/* Compute symetric on 4 first and end columns */
	for (int line=0; line<height; line++) {
		as_2dim_table_double(X_sym_col, height)[0][line]
		        = as_2dim_table_double(chan_YUV, height)[4][line];
		as_2dim_table_double(X_sym_col, height)[1][line]
		        = as_2dim_table_double(chan_YUV, height)[3][line];
		as_2dim_table_double(X_sym_col, height)[2][line]
		        = as_2dim_table_double(chan_YUV, height)[2][line];
		as_2dim_table_double(X_sym_col, height)[3][line]
		        = as_2dim_table_double(chan_YUV, height)[1][line];

		as_2dim_table_double(X_sym_col, height)[width+4][line]
		        = as_2dim_table_double(chan_YUV, height)[width-2][line];
		as_2dim_table_double(X_sym_col, height)[width+5][line]
		        = as_2dim_table_double(chan_YUV, height)[width-3][line];
		as_2dim_table_double(X_sym_col, height)[width+6][line]
		        = as_2dim_table_double(chan_YUV, height)[width-4][line];
		as_2dim_table_double(X_sym_col, height)[width+7][line]
		        = as_2dim_table_double(chan_YUV, height)[width-5][line];
	}

	/* Fil in other cases */
	for (int col=0; col<width; col++) {
		for (int line=0; line<height; line++) {
			as_2dim_table_double(X_sym_col, height)[col+4][line] = as_2dim_table_double(chan_YUV, height)[col][line];
		}
	}

	/*
	FILE *fp1;
	fp1 = fopen("X_Sym_Col.csv", "wb");
	for(int row = 0; row < (height); row++){
		for(int col = 0; col < width; col++){
			fprintf(fp1, "%lf, ", as_2dim_table_double(X_sym_col,((height)/2))[col][row]);
		}
		fprintf(fp1,"\n");
	}
	fclose(fp1);
	*/
	
	double *tmp_LP1 = calloc(height*width/2, sizeof(double));
	double *tmp_HP1 = calloc(height*width/2, sizeof(double));

	for (int col=4; col<width+3; col += 2) {
		for (int line=0; line<height; line++) {
			unsigned int ind = (col+1-4)/2;
			as_2dim_table_double(tmp_LP1, height)[ind][line] =
			        as_2dim_table_double(X_sym_col, height)[col-4][line]*LP_Filter[0] +
			        as_2dim_table_double(X_sym_col, height)[col-3][line]*LP_Filter[1] +
			        as_2dim_table_double(X_sym_col, height)[col-2][line]*LP_Filter[2] +
			        as_2dim_table_double(X_sym_col, height)[col-1][line]*LP_Filter[3] +
			        as_2dim_table_double(X_sym_col, height)[col][line]*  LP_Filter[4] +
			        as_2dim_table_double(X_sym_col, height)[col+1][line]*LP_Filter[5] +
			        as_2dim_table_double(X_sym_col, height)[col+2][line]*LP_Filter[6] +
			        as_2dim_table_double(X_sym_col, height)[col+3][line]*LP_Filter[7] +
			        as_2dim_table_double(X_sym_col, height)[col+4][line]*LP_Filter[8];

			as_2dim_table_double(tmp_HP1, height)[ind][line] =
			        as_2dim_table_double(X_sym_col, height)[col+1-3][line]*HP_Filter[0] +
			        as_2dim_table_double(X_sym_col, height)[col+1-2][line]*HP_Filter[1] +
			        as_2dim_table_double(X_sym_col, height)[col+1-1][line]*HP_Filter[2] +
			        as_2dim_table_double(X_sym_col, height)[col+1][line]  *HP_Filter[3] +
			        as_2dim_table_double(X_sym_col, height)[col+1+1][line]*HP_Filter[4] +
			        as_2dim_table_double(X_sym_col, height)[col+1+2][line]*HP_Filter[5] +
			        as_2dim_table_double(X_sym_col, height)[col+1+3][line]*HP_Filter[6];
		}
	}


//	fprintf(stdout, "%s LP1 :\n", __func__);
//	print_matrix_TL(tmp_LP1, height);

//	fprintf(stdout, "%s HP1 :\n", __func__);
//	print_matrix_TL(tmp_HP1, height);


double *X_1 = calloc((height)*width, sizeof(double));
	for (int col = 0; col < width; col++){
		for (int line=0; line < height; line++) {
			if (col < width/2 ){
				as_2dim_table_double(X_1, height)[col][line]
			        = as_2dim_table_double(tmp_LP1, height)[col][line];
			}
			else{
				as_2dim_table_double(X_1, height)[col][line]
			        = as_2dim_table_double(tmp_HP1, height)[col-width/2][line];
			}
		}
	}

//	fprintf(stdout, "\n%s X_1 :\n", __func__);
//	print_matrix_TL(X_1, height);
//	fprintf(stdout, "\n");
//	print_matrix_TR(X_1, height);
//	fprintf(stdout, "\n");
//	print_matrix_BL(X_1, height);
//	fprintf(stdout, "\n");
//	print_matrix_BR(X_1, height);
//	fprintf(stdout, "\n");


	double *X_sym_line = calloc((height+8)*width, sizeof(double));
	/* Compute symetric on 4 first and end lines */
	for (int col=0; col<(width); col++) {
		as_2dim_table_double(X_sym_line, height+8)[col][0]
		        = as_2dim_table_double(X_1, height)[col][4];
		as_2dim_table_double(X_sym_line, height+8)[col][1]
		        = as_2dim_table_double(X_1, height)[col][3];
		as_2dim_table_double(X_sym_line, height+8)[col][2]
		        = as_2dim_table_double(X_1, height)[col][2];
		as_2dim_table_double(X_sym_line, height+8)[col][3]
		        = as_2dim_table_double(X_1, height)[col][1];

		as_2dim_table_double(X_sym_line, height+8)[col][height+8-4]
		        = as_2dim_table_double(X_1, height)[col][height-2];
		as_2dim_table_double(X_sym_line, height+8)[col][height+8-3]
		        = as_2dim_table_double(X_1, height)[col][height-3];
		as_2dim_table_double(X_sym_line, height+8)[col][height+8-2]
		        = as_2dim_table_double(X_1, height)[col][height-4];
		as_2dim_table_double(X_sym_line, height+8)[col][height+8-1]
		        = as_2dim_table_double(X_1, height)[col][height-5];
	
	}
	for (int col=0; col<width; col++) {
		for (int line=0; line<height; line++) {
			as_2dim_table_double(X_sym_line, height+8)[col][line+4]
			           = as_2dim_table_double(X_1, height)[col][line];
		}
	}
	/*Print data to a csv and open in excel to compare to matlab results*/	
/*	FILE *fp;
	fp = fopen("X_sim_line.csv", "wb");
	for(int row = 0; row < height+8; row++){
		for(int col = 0; col < width; col++){
			fprintf(fp, "%lf,\t ", as_2dim_table_double(X_sym_line, height+8)[col][row]);
		}
	fprintf(fp,"\n");
	}
	fclose(fp);*/
	
/*	fprintf(stdout, "%s Sym_Line :\n", __func__);
	print_matrix_TL(X_sym_line, height+8);
	fprintf(stdout, "\n");
	print_matrix_BL(X_sym_line, height+8);
	fprintf(stdout, "\n");*/

	double *tmp_LP2 = calloc(height/2*width, sizeof(double));
	double *tmp_HP2 = calloc(height/2*width, sizeof(double));
/*Second round of filtering*/
	for (int line=4; line<height+3; line += 2) {
		unsigned int ind = (line+1-4)/2;
		for (int col=0; col<width; col++) {
			as_2dim_table_double(tmp_LP2, (height)/2)[col][ind] =
			        as_2dim_table_double(X_sym_line, height+8)[col][line-4]*LP_Filter[0] +
			        as_2dim_table_double(X_sym_line, height+8)[col][line-3]*LP_Filter[1] +
			        as_2dim_table_double(X_sym_line, height+8)[col][line-2]*LP_Filter[2] +
			        as_2dim_table_double(X_sym_line, height+8)[col][line-1]*LP_Filter[3] +
			        as_2dim_table_double(X_sym_line, height+8)[col][line]  *LP_Filter[4] +
			        as_2dim_table_double(X_sym_line, height+8)[col][line+1]*LP_Filter[5] +
			        as_2dim_table_double(X_sym_line, height+8)[col][line+2]*LP_Filter[6] +
			        as_2dim_table_double(X_sym_line, height+8)[col][line+3]*LP_Filter[7] +
			        as_2dim_table_double(X_sym_line, height+8)[col][line+4]*LP_Filter[8];
			as_2dim_table_double(tmp_HP2, (height)/2)[col][ind] =
			        as_2dim_table_double(X_sym_line, height+8)[col][line+1-3]*HP_Filter[0] +
			        as_2dim_table_double(X_sym_line, height+8)[col][line+1-2]*HP_Filter[1] +
			        as_2dim_table_double(X_sym_line, height+8)[col][line+1-1]*HP_Filter[2] +
			        as_2dim_table_double(X_sym_line, height+8)[col][line+1]  *HP_Filter[3] +
			        as_2dim_table_double(X_sym_line, height+8)[col][line+1+1]*HP_Filter[4] +
			        as_2dim_table_double(X_sym_line, height+8)[col][line+1+2]*HP_Filter[5] +
			        as_2dim_table_double(X_sym_line, height+8)[col][line+1+3]*HP_Filter[6];
		}
	}
/*	FILE *fp1;
	fp1 = fopen("temp_LP2.csv", "wb");
	for(int row = 0; row < (height)/2; row++){
		for(int col = 0; col < width; col++){
			fprintf(fp1, "%lf, ", as_2dim_table_double(tmp_LP2,((height)/2))[col][row]);
		}
		fprintf(fp1,"\n");
	}
	fclose(fp1);*/

/*	FILE *fp2;
	fp2 = fopen("temp_HP2.csv", "wb");
	for(int row = 0; row < (height)/2; row++){
		for(int col = 0; col < width; col++){
			fprintf(fp2, "%lf, ", as_2dim_table_double(tmp_HP2,((height)/2))[col][row]);
		}
		fprintf(fp2,"\n");
	}
	fclose(fp2);*/

	/*fprintf(stdout, "%s LP2 :\n", __func__);
	print_matrix_TL(tmp_LP2, (height+4)/2);
	fprintf(stdout, "\n");
	print_matrix_BL(tmp_LP2, (height+4)/2);
	fprintf(stdout, "\n");
	fprintf(stdout, "%s HP2 :\n", __func__);
	print_matrix_TL(tmp_HP2, (height+8)/2);
	fprintf(stdout, "\n");
	print_matrix_BL(tmp_HP2, (height+8)/2);
	fprintf(stdout, "\n");*/

/*place LP2 above HP2 in the same matrix*/
	for (int col = 0; col < width; col++){
		for (int line=0; line < height; line++) {
			if (line < height/2 ){
				as_2dim_table_double(chan_YUV, height)[col][line]
			        = as_2dim_table_double(tmp_LP2, height/2)[col][line];
			}
			else{
				as_2dim_table_double(chan_YUV, height)[col][line]
			        = as_2dim_table_double(tmp_HP2, height/2)[col][line-(height/2)];
			}
		}
	}
/*	FILE *fp3;
	fp3 = fopen("chan_YUV.csv", "wb");
	for(int row = 0; row < height; row++){
		for(int col = 0; col < width; col++){
			fprintf(fp3, "%lf, ", as_2dim_table_double(chan_YUV,height)[col][row]);
		}
		fprintf(fp3,"\n");
	}
	fclose(fp3);*/
}


int demo_malvar(unsigned char *bayer_img, unsigned int width, unsigned int height, unsigned char *result_r, unsigned char *result_g, unsigned char *result_b)
{
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


	/* Malvar */
	double *G = calloc(width*height, sizeof(double));
	double *R = calloc(width*height, sizeof(double));
	double *B = calloc(width*height, sizeof(double));
	for (unsigned int line=0; line<=(height - 3); line = line + 2) {
		for (unsigned int col=0; col<=(width - 3); col = col + 2) {
			as_2dim_table_double(G, 1024)[col+1][line] = as_2dim_table_uchar(bayer_img, 1024)[col+1][line];
			as_2dim_table_double(G, 1024)[col][line+1] = as_2dim_table_uchar(bayer_img, 1024)[col][line+1];
			as_2dim_table_double(R, 1024)[col][line] = as_2dim_table_uchar(bayer_img, 1024)[col][line];
			as_2dim_table_double(B, 1024)[col+1][line+1] = as_2dim_table_uchar(bayer_img, 1024)[col+1][line+1];
			/* Allow to skip the edges of the image (2x2) */
			if (line >= 2 && line <= height-3 && col >= 2 && col <= width-3) {
				/* Green channel */
				as_2dim_table_double(G, 1024)[col][line] = interpolation_Malvar(bayer_img, G_at_RB, line-2, col-2);
				as_2dim_table_double(G, 1024)[col+1][line+1] = interpolation_Malvar(bayer_img, G_at_RB, line-1, col-1);
				/* Red channel */
				as_2dim_table_double(R, 1024)[col+1][line] = interpolation_Malvar(bayer_img, R1, line-2, col-1);
				as_2dim_table_double(R, 1024)[col][line+1] = interpolation_Malvar(bayer_img, R2, line-1, col-2);
				as_2dim_table_double(R, 1024)[col+1][line+1] = interpolation_Malvar(bayer_img, R3, line-1, col-1);
				/* Blue channel */
				as_2dim_table_double(B, 1024)[col][line] = interpolation_Malvar(bayer_img, B3, line-2, col-2);
				as_2dim_table_double(B, 1024)[col+1][line] = interpolation_Malvar(bayer_img, B2, line-2, col-1);
				as_2dim_table_double(B, 1024)[col][line+1] = interpolation_Malvar(bayer_img, B1, line-1, col-2);
			}
		}
	}

	fprintf(stdout, "R :\n");
	print_matrix_TL(R, height);
	convert_array_double_to_uchar(R, result_r, width*height);

	fprintf(stdout, "G :\n");
	print_matrix_TL(G, height);
	convert_array_double_to_uchar(G, result_g, width*height);

	fprintf(stdout, "B :\n");
	print_matrix_TL(B, height);
	convert_array_double_to_uchar(B, result_b, width*height);


	/* Anscombe */
	double *R_cut = calloc((width-4)*(height-4), sizeof(double));
	double *G_cut = calloc((width-4)*(height-4), sizeof(double));
	double *B_cut = calloc((width-4)*(height-4), sizeof(double));
	/* Sum of column members sum */
	for (int line=0; line<width-4; line++) {
		/* Sum of column members */
		for (int col=0; col<height-4; col++) {
			as_2dim_table_double(G_cut, height-4)[col][line]
			      = anscombe_trf(as_2dim_table_double(G, height)[col+2][line+2], 0.1, 0.2);
			as_2dim_table_double(R_cut, height-4)[col][line]
			      = anscombe_trf(as_2dim_table_double(R, height)[col+2][line+2], 0.1, 0.2);
			as_2dim_table_double(B_cut, height-4)[col][line]
			      = anscombe_trf(as_2dim_table_double(B, height)[col+2][line+2], 0.1, 0.2);
		}
	}


	double *YUV_1 = calloc((width-4)*(height-4), sizeof(double));
	double *YUV_2 = calloc((width-4)*(height-4), sizeof(double));
	double *YUV_3 = calloc((width-4)*(height-4), sizeof(double));
	/* Convert RGB to YUV */
	for (int line=0; line<width-4; line++) {
		/* Sum of column members */
		for (int col=0; col<height-4; col++) {
			/* TODO inverse R and G if CNES error */
			RGB_to_YUV(as_2dim_table_double(G_cut, height-4)[col][line],
			           as_2dim_table_double(R_cut, height-4)[col][line],
			           as_2dim_table_double(B_cut, height-4)[col][line],
			           &as_2dim_table_double(YUV_1, height-4)[col][line],
			           &as_2dim_table_double(YUV_2, height-4)[col][line],
			           &as_2dim_table_double(YUV_3, height-4)[col][line]);
		}
	}

/*	fprintf(stdout, "YUV 1 :\n");
	print_matrix_TL(YUV_1, height-4);

	fprintf(stdout, "YUV 2 :\n");
	print_matrix_TL(YUV_2, height-4);

	fprintf(stdout, "YUV 3 :\n");
	print_matrix_TL(YUV_3, height-4);*/

	DWT_CCSDS_single_level(YUV_1, height-4, width-4);
//	DWT_CCSDS_single_level(YUV_2, height-4, width-4);
//	DWT_CCSDS_single_level(YUV_3, height-4, width-4);

	double *Y_BF = calloc(513*513, sizeof(double));
	for(int row = 0; row < 513; row++){
		for(int col = 0; col < 513; col++){
			as_2dim_table_double(Y_BF,513)[col][row] = as_2dim_table_double(YUV_1,height-4)[col][row];
		}
	}

	return 0;
}



/****************************************************/
/**************** Utility functions *****************/
/****************************************************/
void load_img(char *path, unsigned char *image, unsigned int size)
{
	FILE *ptr;

	/* Open file */
	ptr = fopen(path,"rb");
	if (ptr == NULL) {
		perror("Cannot open file");
	}

	/* Load into memory and close file */
	fread(image, size, 1, ptr);
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
	fwrite(image, size, 1, ptr);
	fclose(ptr);

	fprintf(stdout, "File %s opened, size=%u bytes\n", path, size);
}

void print_matrix_TL(double *matrix, unsigned int height)
{
	for (int i=0; i<10; i++) {
		for (int j=0; j<10; j++) {
			fprintf(stdout, "%.3lf\t", as_2dim_table_double(matrix, height)[j][i]);
		}
		fprintf(stdout, "\n");
	}
}

void print_matrix_TR(double *matrix, unsigned int height)
{
	for (int i=0; i<10; i++) {
		for (int j=(1048576/height)-10; j<(1048576/height); j++) {
			fprintf(stdout, "%.3lf\t", as_2dim_table_double(matrix, height)[j][i]);
		}
		fprintf(stdout, "\n");
	}
}

void print_matrix_BL(double *matrix, unsigned int height)
{
	for (int i=height-10; i<height; i++) {
		for (int j=0; j<10; j++) {
			fprintf(stdout, "%.3lf\t", as_2dim_table_double(matrix, height)[j][i]);
		}
		fprintf(stdout, "\n");
	}
}

void print_matrix_BR(double *matrix, unsigned int height)
{
	for (int i=height-10; i<height; i++) {
		for (int j=height-10; j<height; j++) {
			fprintf(stdout, "%.3lf\t", as_2dim_table_double(matrix, height)[j][i]);
		}
		fprintf(stdout, "\n");
	}
}

void convert_array_double_to_uchar(double *input_array, unsigned char *output_array, unsigned int size)
{
	for (int i=0; i<size; i++) {
		output_array[i] = (unsigned char)round(input_array[i]);
	}
}


/****************************************************/
/****************** Main function *******************/
/****************************************************/
int main(int argc, char *argv[])
{
	unsigned char *imagette;
	unsigned int imagette_width;
	unsigned int imagette_height;

	unsigned char *result_r;
	unsigned char *result_g;
	unsigned char *result_b;
	unsigned int result_width;
	unsigned int result_height;

	printf("CNES Image Processing Algorithm\n");

	if (argc != 7)
	{
		printf("Mauvais arguments\n");
		return -1;
	}

	imagette_width = atoi(argv[2]);
	imagette_height = atoi(argv[3]);

	result_width = imagette_width;
	result_height = imagette_height;

	imagette = calloc(imagette_width*imagette_height, sizeof(unsigned char));
	load_img(argv[1], imagette, imagette_width*imagette_height);

	result_r = calloc(result_width*result_height, sizeof(unsigned char));
	result_g = calloc(result_width*result_height, sizeof(unsigned char));
	result_b = calloc(result_width*result_height, sizeof(unsigned char));

	demo_malvar(imagette, imagette_width, imagette_height, result_r, result_g, result_b);

	save_img(argv[4], result_r, result_width*result_height);
	save_img(argv[5], result_g, result_width*result_height);
	save_img(argv[6], result_b, result_width*result_height);

	free(imagette);
	free(result_r);
	free(result_g);
	free(result_b);

	return 0;
}
