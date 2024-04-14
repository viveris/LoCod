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

#include "demosaicing_float.h"
#include <math.h>

#define as_2dim_table_uchar(ptr, height) ((unsigned char (*)[height])ptr)
#define as_2dim_table_float(ptr, height) ((float (*)[height])ptr)
#define as_2dim_table_double(ptr, height) ((double (*)[height])ptr)

void print_matrix_TL(float *matrix, unsigned int height)
{
	for (int i=0; i<10; i++) {
		for (int j=0; j<10; j++) {
			fprintf(stdout, "%.3lf\t", as_2dim_table_float(matrix, height)[j][i]);
		}
		fprintf(stdout, "\n");
	}
}

void print_matrix_TR(float *matrix, unsigned int height)
{
	for (int i=0; i<10; i++) {
		for (int j=height-10; j<height-10; j++) {
			fprintf(stdout, "%.3lf\t", as_2dim_table_float(matrix, height)[j][i]);
		}
		fprintf(stdout, "\n");
	}
}

void print_matrix_BL(float *matrix, unsigned int height)
{
	for (int i=height-10; i<height; i++) {
		for (int j=0; j<10; j++) {
			fprintf(stdout, "%.3lf\t", as_2dim_table_float(matrix, height)[j][i]);
		}
		fprintf(stdout, "\n");
	}
}

void print_matrix_BR(float *matrix, unsigned int height)
{
	for (int i=height-10; i<height; i++) {
		for (int j=height-10; j<height; j++) {
			fprintf(stdout, "%.3lf\t", as_2dim_table_float(matrix, height)[j][i]);
		}
		fprintf(stdout, "\n");
	}
}

/****************************************************/
/***************** Algo functions *******************/
/****************************************************/
float interpolation_Malvar(unsigned char *matrix,
                           float kernel[5][5],
                           unsigned int line_start,
                           unsigned int col_start,
						   unsigned int height)
{
	float sum_col = 0;
	float result = 0;

	/* Sum of column members sum */
	for (int line=line_start; line<line_start+5; line++) {
		/* Sum of column members */
		for (int col=col_start; col<col_start+5; col++) {
			sum_col += as_2dim_table_uchar(matrix, height)[col][line] * kernel[col-col_start][line-line_start];
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

float anscombe_trf(float pixel, float ca, float cb)
{
	return (2/cb)*sqrt(cb*pixel + 3*pow(cb, 2)/8 + pow(ca, 2));
}

float *RGB_to_YUV(float R,
                   float G,
                   float B,
                   float *YUV_1,
                   float *YUV_2,
                   float *YUV_3)
{
	*YUV_1 = 0.299*R + 0.587*G + 0.114*B;
	*YUV_2 = 128 - 0.168736*R - 0.331264*G + 0.5*B;
	*YUV_3 = 128 + 0.5*R - 0.418688*G - 0.081312*B;
}

void DWT_CCSDS_single_level(float *chan_YUV, unsigned int height, unsigned int width)
{
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

	float LP_InvFilter[7] = {-0.064538882629,
	                          -0.040689417609,
	                           0.418092273222,
	                           0.788485616406,
	                           0.418092273222,
	                          -0.040689417609,
	                          -0.064538882629};

	float HP_InvFilter[9] = {-0.037828455507,
	                          -0.023849465020,
	                           0.110624404418,
	                           0.377402855613,
	                          -0.852698679009,
	                           0.377402855613,
	                           0.110624404418,
	                          -0.023849465020,
	                          -0.037828455507};

#ifdef DEBUG
/*	FILE *fp_YUV;
	fp_YUV = fopen("chan_YUV.csv", "wb");
	for(int row = 0; row < height; row++){
		for(int col = 0; col < width; col++){
			fprintf(fp_YUV, "%lf,\t ", as_2dim_table_float(chan_YUV, height)[col][row]);
		}
	fprintf(fp_YUV,"\n");
	}
	fclose(fp_YUV);*/
#endif /* DEBUG */
	float *X_sym_col = calloc(height*(width+8), sizeof(float));
	/* Compute symetric on 4 first and end columns */
	for (int line=0; line<height; line++) {
		as_2dim_table_float(X_sym_col, height)[0][line]
		        = as_2dim_table_float(chan_YUV, height)[4][line];
		as_2dim_table_float(X_sym_col, height)[1][line]
		        = as_2dim_table_float(chan_YUV, height)[3][line];
		as_2dim_table_float(X_sym_col, height)[2][line]
		        = as_2dim_table_float(chan_YUV, height)[2][line];
		as_2dim_table_float(X_sym_col, height)[3][line]
		        = as_2dim_table_float(chan_YUV, height)[1][line];

		as_2dim_table_float(X_sym_col, height)[width+4][line]
		        = as_2dim_table_float(chan_YUV, height)[width-2][line];
		as_2dim_table_float(X_sym_col, height)[width+5][line]
		        = as_2dim_table_float(chan_YUV, height)[width-3][line];
		as_2dim_table_float(X_sym_col, height)[width+6][line]
		        = as_2dim_table_float(chan_YUV, height)[width-4][line];
		as_2dim_table_float(X_sym_col, height)[width+7][line]
		        = as_2dim_table_float(chan_YUV, height)[width-5][line];
	}

	/* Fil in other cases */
	for (int col=0; col<width; col++) {
		for (int line=0; line<height; line++) {
			as_2dim_table_float(X_sym_col, height)[col+4][line] = as_2dim_table_float(chan_YUV, height)[col][line];
		}
	}
#ifdef DEBUG
/*	FILE *fp_col;
	fp_col = fopen("X_sim_col.csv", "wb");
	for(int row = 0; row < height; row++){
		for(int col = 0; col < width+8; col++){
			fprintf(fp_col, "%lf,\t ", as_2dim_table_float(X_sym_col, height)[col][row]);
		}
	fprintf(fp_col,"\n");
	}
	fclose(fp_col);*/
#endif /* DEBUG */

	float *tmp_LP1 = calloc(height*width/2, sizeof(float));
	float *tmp_HP1 = calloc(height*width/2, sizeof(float));

	for (int col=4; col<width+3; col += 2) {
		for (int line=0; line<height; line++) {
			unsigned int ind = (col+1-4)/2;
			as_2dim_table_float(tmp_LP1, height)[ind][line] =
			        as_2dim_table_float(X_sym_col, height)[col-4][line]*LP_Filter[0] +
			        as_2dim_table_float(X_sym_col, height)[col-3][line]*LP_Filter[1] +
			        as_2dim_table_float(X_sym_col, height)[col-2][line]*LP_Filter[2] +
			        as_2dim_table_float(X_sym_col, height)[col-1][line]*LP_Filter[3] +
			        as_2dim_table_float(X_sym_col, height)[col][line]*  LP_Filter[4] +
			        as_2dim_table_float(X_sym_col, height)[col+1][line]*LP_Filter[5] +
			        as_2dim_table_float(X_sym_col, height)[col+2][line]*LP_Filter[6] +
			        as_2dim_table_float(X_sym_col, height)[col+3][line]*LP_Filter[7] +
			        as_2dim_table_float(X_sym_col, height)[col+4][line]*LP_Filter[8];

			as_2dim_table_float(tmp_HP1, height)[ind][line] =
			        as_2dim_table_float(X_sym_col, height)[col+1-3][line]*HP_Filter[0] +
			        as_2dim_table_float(X_sym_col, height)[col+1-2][line]*HP_Filter[1] +
			        as_2dim_table_float(X_sym_col, height)[col+1-1][line]*HP_Filter[2] +
			        as_2dim_table_float(X_sym_col, height)[col+1][line]  *HP_Filter[3] +
			        as_2dim_table_float(X_sym_col, height)[col+1+1][line]*HP_Filter[4] +
			        as_2dim_table_float(X_sym_col, height)[col+1+2][line]*HP_Filter[5] +
			        as_2dim_table_float(X_sym_col, height)[col+1+3][line]*HP_Filter[6];
		}
	}

float *X_1 = calloc((height)*width, sizeof(float));
	for (int col = 0; col < width; col++){
		for (int line=0; line < height; line++) {
			if (col < width/2 ){
				as_2dim_table_float(X_1, height)[col][line]
			        = as_2dim_table_float(tmp_LP1, height)[col][line];
			}
			else{
				as_2dim_table_float(X_1, height)[col][line]
			        = as_2dim_table_float(tmp_HP1, height)[col-width/2][line];
			}
		}
	}

float *X_sym_line = calloc((height+8)*width, sizeof(float));
	/* Compute symetric on 4 first and end lines */
	for (int col=0; col<(width); col++) {
		as_2dim_table_float(X_sym_line, height+8)[col][0]
		        = as_2dim_table_float(X_1, height)[col][4];
		as_2dim_table_float(X_sym_line, height+8)[col][1]
		        = as_2dim_table_float(X_1, height)[col][3];
		as_2dim_table_float(X_sym_line, height+8)[col][2]
		        = as_2dim_table_float(X_1, height)[col][2];
		as_2dim_table_float(X_sym_line, height+8)[col][3]
		        = as_2dim_table_float(X_1, height)[col][1];

		as_2dim_table_float(X_sym_line, height+8)[col][height+8-4]
		        = as_2dim_table_float(X_1, height)[col][height-2];
		as_2dim_table_float(X_sym_line, height+8)[col][height+8-3]
		        = as_2dim_table_float(X_1, height)[col][height-3];
		as_2dim_table_float(X_sym_line, height+8)[col][height+8-2]
		        = as_2dim_table_float(X_1, height)[col][height-4];
		as_2dim_table_float(X_sym_line, height+8)[col][height+8-1]
		        = as_2dim_table_float(X_1, height)[col][height-5];
	
	}
	for (int col=0; col<width; col++) {
		for (int line=0; line<height; line++) {
			as_2dim_table_float(X_sym_line, height+8)[col][line+4]
			           = as_2dim_table_float(X_1, height)[col][line];
		}
	}
#ifdef DEBUG	
	/*Print data to a csv and open in excel to compare to matlab results*/	
	/*FILE *fp;
	fp = fopen("X_sim_line.csv", "wb");
	for(int row = 0; row < height+8; row++){
		for(int col = 0; col < width; col++){
			fprintf(fp, "%lf,\t ", as_2dim_table_float(X_sym_line, height+8)[col][row]);
		}
	fprintf(fp,"\n");
	}
	fclose(fp);*/
#endif /* DEBUG */	


	float *tmp_LP2 = calloc(height/2*width, sizeof(float));
	float *tmp_HP2 = calloc(height/2*width, sizeof(float));
/*Second round of filtering*/
	for (int line=4; line<height+3; line += 2) {
		unsigned int ind = (line+1-4)/2;
		for (int col=0; col<width; col++) {
			as_2dim_table_float(tmp_LP2, (height)/2)[col][ind] =
			        as_2dim_table_float(X_sym_line, height+8)[col][line-4]*LP_Filter[0] +
			        as_2dim_table_float(X_sym_line, height+8)[col][line-3]*LP_Filter[1] +
			        as_2dim_table_float(X_sym_line, height+8)[col][line-2]*LP_Filter[2] +
			        as_2dim_table_float(X_sym_line, height+8)[col][line-1]*LP_Filter[3] +
			        as_2dim_table_float(X_sym_line, height+8)[col][line]  *LP_Filter[4] +
			        as_2dim_table_float(X_sym_line, height+8)[col][line+1]*LP_Filter[5] +
			        as_2dim_table_float(X_sym_line, height+8)[col][line+2]*LP_Filter[6] +
			        as_2dim_table_float(X_sym_line, height+8)[col][line+3]*LP_Filter[7] +
			        as_2dim_table_float(X_sym_line, height+8)[col][line+4]*LP_Filter[8];
			as_2dim_table_float(tmp_HP2, (height)/2)[col][ind] =
			        as_2dim_table_float(X_sym_line, height+8)[col][line+1-3]*HP_Filter[0] +
			        as_2dim_table_float(X_sym_line, height+8)[col][line+1-2]*HP_Filter[1] +
			        as_2dim_table_float(X_sym_line, height+8)[col][line+1-1]*HP_Filter[2] +
			        as_2dim_table_float(X_sym_line, height+8)[col][line+1]  *HP_Filter[3] +
			        as_2dim_table_float(X_sym_line, height+8)[col][line+1+1]*HP_Filter[4] +
			        as_2dim_table_float(X_sym_line, height+8)[col][line+1+2]*HP_Filter[5] +
			        as_2dim_table_float(X_sym_line, height+8)[col][line+1+3]*HP_Filter[6];
		}
	}
#ifdef DEBUG
/*	FILE *fp1;
	fp1 = fopen("temp_LP2.csv", "wb");
	for(int row = 0; row < (height)/2; row++){
		for(int col = 0; col < width; col++){
			fprintf(fp1, "%lf, ", as_2dim_table_float(tmp_LP2,((height)/2))[col][row]);
		}
		fprintf(fp1,"\n");
	}
	fclose(fp1);

	FILE *fp2;
	fp2 = fopen("temp_HP2.csv", "wb");
	for(int row = 0; row < (height)/2; row++){
		for(int col = 0; col < width; col++){
			fprintf(fp2, "%lf, ", as_2dim_table_float(tmp_HP2,((height)/2))[col][row]);
		}
		fprintf(fp2,"\n");
	}
	fclose(fp2);*/
#endif /* DEBUG */

/*place LP2 above HP2 in the same matrix*/
	for (int col = 0; col < width; col++){
		for (int line=0; line < height; line++) {
			if (line < height/2 ){
				as_2dim_table_float(chan_YUV, height)[col][line]
			        = as_2dim_table_float(tmp_LP2, height/2)[col][line];
			}
			else{
				as_2dim_table_float(chan_YUV, height)[col][line]
			        = as_2dim_table_float(tmp_HP2, height/2)[col][line-(height/2)];
			}
		}
	}
#ifdef DEBUG
/*	FILE *fp3;
	fp3 = fopen("chan_YUV.csv", "wb");
	for(int row = 0; row < height; row++){
		for(int col = 0; col < width; col++){
			fprintf(fp3, "%lf, ", as_2dim_table_float(chan_YUV,height)[col][row]);
		}
		fprintf(fp3,"\n");
	}
	fclose(fp3);*/
#endif /* DEBUG */
}

/* unsigned char *demo_malvar(unsigned char *bayer_img, unsigned int width, unsigned int height)
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
	                 };*/


	
	/* Malvar */
	/*float *G = calloc(width*height, sizeof(float));
	float *R = calloc(width*height, sizeof(float));
	float *B = calloc(width*height, sizeof(float));
	for (unsigned int line=0; line<(height - 2); line = line + 2) {
		for (unsigned int col=0; col<(width - 2); col = col + 2) {
			as_2dim_table_float(G, 1024)[col+1][line] = as_2dim_table_uchar(bayer_img, 1024)[col+1][line];
			as_2dim_table_float(G, 1024)[col][line+1] = as_2dim_table_uchar(bayer_img, 1024)[col][line+1];
			as_2dim_table_float(R, 1024)[col][line] = as_2dim_table_uchar(bayer_img, 1024)[col][line];
			as_2dim_table_float(B, 1024)[col+1][line+1] = as_2dim_table_uchar(bayer_img, 1024)[col+1][line+1];*/
			/* Allow to skip the edges of the image (2x2) */
//			if (line >= 2 && line <= height-3 && col >= 2 && col <= width-3) {
				/* Green channel */
//				as_2dim_table_float(G, 1024)[col][line] = interpolation_Malvar(bayer_img, G_at_RB, line-2, col-2);
//				as_2dim_table_float(G, 1024)[col+1][line+1] = interpolation_Malvar(bayer_img, G_at_RB, line-1, col-1);
				/* Red channel */
//				as_2dim_table_float(R, 1024)[col+1][line] = interpolation_Malvar(bayer_img, R1, line-2, col-1);
//				as_2dim_table_float(R, 1024)[col][line+1] = interpolation_Malvar(bayer_img, R2, line-1, col-2);
//				as_2dim_table_float(R, 1024)[col+1][line+1] = interpolation_Malvar(bayer_img, R3, line-1, col-1);
				/* Blue channel */
//				as_2dim_table_float(B, 1024)[col][line] = interpolation_Malvar(bayer_img, B3, line-2, col-2);
//				as_2dim_table_float(B, 1024)[col+1][line] = interpolation_Malvar(bayer_img, B2, line-2, col-1);
//				as_2dim_table_float(B, 1024)[col][line+1] = interpolation_Malvar(bayer_img, B1, line-1, col-2);
//			}
//		}
//	}
#ifdef DEBUG
	/*FILE *fp_Red;
	fp_Red = fopen("Red.csv", "wb");
	for(int row = 0; row < height; row++){
		for(int col = 0; col < width; col++){
			fprintf(fp_Red, "%lf,\t ", as_2dim_table_float(R, height)[col][row]);
		}
	fprintf(fp_Red,"\n");
	}
	fclose(fp_Red);*/
#endif /* DEBUG */

/*	float *R_cut = calloc((width-4)*(height-4), sizeof(float));
	float *G_cut = calloc((width-4)*(height-4), sizeof(float));
	float *B_cut = calloc((width-4)*(height-4), sizeof(float));*/
	/* Anscombe */
	/* Sum of column members sum */
//	for (int line=0; line<width-4; line++) {
		/* Sum of column members */
/*		for (int col=0; col<height-4; col++) {
			as_2dim_table_float(G_cut, height-4)[col][line]
			      = anscombe_trf(as_2dim_table_float(G, height)[col+2][line+2], 0.1, 0.2);
			as_2dim_table_float(R_cut, height-4)[col][line]
			      = anscombe_trf(as_2dim_table_float(R, height)[col+2][line+2], 0.1, 0.2);
			as_2dim_table_float(B_cut, height-4)[col][line]
			      = anscombe_trf(as_2dim_table_float(B, height)[col+2][line+2], 0.1, 0.2);
		}
	}*/

/*	float *YUV_1 = calloc((width-4)*(height-4), sizeof(float));
	float *YUV_2 = calloc((width-4)*(height-4), sizeof(float));
	float *YUV_3 = calloc((width-4)*(height-4), sizeof(float));*/
	/* Convert RGB to YUV */
//	for (int line=0; line<width-4; line++) {
		/* Sum of column members */
//		for (int col=0; col<height-4; col++) {
			/* TODO inverse R and G if CNES error */
/*			RGB_to_YUV(as_2dim_table_float(G_cut, height-4)[col][line],
			           as_2dim_table_float(R_cut, height-4)[col][line],
			           as_2dim_table_float(B_cut, height-4)[col][line],
			           &as_2dim_table_float(YUV_1, height-4)[col][line],
			           &as_2dim_table_float(YUV_2, height-4)[col][line],
			           &as_2dim_table_float(YUV_3, height-4)[col][line]);
		}
	}*/

/*	fprintf(stdout, "YUV 1 :\n");
	print_matrix_TL(YUV_1, height-4);

	fprintf(stdout, "YUV 2 :\n");
	print_matrix_TL(YUV_2, height-4);

	fprintf(stdout, "YUV 3 :\n");
	print_matrix_TL(YUV_3, height-4);*/

//	DWT_CCSDS_single_level(YUV_1, height-4, width-4);
//	DWT_CCSDS_single_level(YUV_2, height-4, width-4);
//	DWT_CCSDS_single_level(YUV_3, height-4, width-4);
/*float *Y_BF = calloc((513)*(513), sizeof(float));

	
	FILE *fp4;
	fp4 = fopen("Y_BF(float).csv", "wb+");
	for(int row = 0; row < 513; row++){
		for(int col = 0; col < 513; col++){
			fprintf(fp4, "%lf, ", as_2dim_table_float(YUV_1,height-4)[col][row]);
			as_2dim_table_float(Y_BF,513)[col][row] = as_2dim_table_float(YUV_1,height-4)[col][row];
		}
		fprintf(fp4,"\n");
	}
	fclose(fp4);
	fprintf(stdout, ".csv file of result written\n");
/*	FILE *fp5;
	fp5 = fopen("Y_BF(float).bin", "wb+");
	fwrite(Y_BF, sizeof(float), 513*513, fp5);
	/*for(int row = 0; row < 513; row++){
		for(int col = 0; col < 513; col++){
			fp5 = fopen("Y_BF.bin", "a");
			fwrite(as_2dim_table_float(YUV_1,height-4)[col][row], sizeof(float), (height-4)*(height-4), fp5);
		}
	//	fprintf(fp5,"\n");
}*/
/*	fclose(fp5);

	fprintf(stdout, ".bin file of result written\n");

	return NULL;
}*/

unsigned char *malvar_anscombe(	unsigned char *bayer_img, unsigned int width,unsigned int height, 
								float *R_cut, float *G_cut, float *B_cut)
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
	float *G = calloc(width*height, sizeof(float));
	float *R = calloc(width*height, sizeof(float));
	float *B = calloc(width*height, sizeof(float));
	for (unsigned int line=0; line<(height - 2); line = line + 2) {
		for (unsigned int col=0; col<(width - 2); col = col + 2) {
			as_2dim_table_float(G, height)[col+1][line] = as_2dim_table_uchar(bayer_img, height)[col+1][line];
			as_2dim_table_float(G, height)[col][line+1] = as_2dim_table_uchar(bayer_img, height)[col][line+1];
			as_2dim_table_float(R, height)[col][line] = as_2dim_table_uchar(bayer_img, height)[col][line];
			as_2dim_table_float(B, height)[col+1][line+1] = as_2dim_table_uchar(bayer_img, height)[col+1][line+1];
			/* Allow to skip the edges of the image (2x2) */
			if (line >= 2 && line <= height-3 && col >= 2 && col <= width-3) {
				/* Green channel */
				as_2dim_table_float(G, height)[col][line] = interpolation_Malvar(bayer_img, G_at_RB, line-2, col-2, height);
				as_2dim_table_float(G, height)[col+1][line+1] = interpolation_Malvar(bayer_img, G_at_RB, line-1, col-1, height);
				/* Red channel */
				as_2dim_table_float(R, height)[col+1][line] = interpolation_Malvar(bayer_img, R1, line-2, col-1, height);
				as_2dim_table_float(R, height)[col][line+1] = interpolation_Malvar(bayer_img, R2, line-1, col-2, height);
				as_2dim_table_float(R, height)[col+1][line+1] = interpolation_Malvar(bayer_img, R3, line-1, col-1, height);
				/* Blue channel */
				as_2dim_table_float(B, height)[col][line] = interpolation_Malvar(bayer_img, B3, line-2, col-2, height);
				as_2dim_table_float(B, height)[col+1][line] = interpolation_Malvar(bayer_img, B2, line-2, col-1, height);
				as_2dim_table_float(B, height)[col][line+1] = interpolation_Malvar(bayer_img, B1, line-1, col-2, height);
			}
		}
	}
	/*float *R_cut = calloc((width-4)*(height-4), sizeof(float));
	float *G_cut = calloc((width-4)*(height-4), sizeof(float));
	float *B_cut = calloc((width-4)*(height-4), sizeof(float));*/
	/* Anscombe */
	/* Sum of column members sum */
	for (int line=0; line<width-4; line++) {
		/* Sum of column members */
		for (int col=0; col<height-4; col++) {
			as_2dim_table_float(G_cut, height-4)[col][line]
			      = anscombe_trf(as_2dim_table_float(G, height)[col+2][line+2], 0.1, 0.2);
			as_2dim_table_float(R_cut, height-4)[col][line]
			      = anscombe_trf(as_2dim_table_float(R, height)[col+2][line+2], 0.1, 0.2);
			as_2dim_table_float(B_cut, height-4)[col][line]
			      = anscombe_trf(as_2dim_table_float(B, height)[col+2][line+2], 0.1, 0.2);
		}
	}


}

void rearrange_1(float *YUV_1, unsigned int width, unsigned int height, float *X_Sym_Col){
	for (int line=0; line<height; line++) {
		as_2dim_table_float(X_Sym_Col, height)[0][line]
		        = as_2dim_table_float(YUV_1, height)[4][line];
		as_2dim_table_float(X_Sym_Col, height)[1][line]
		        = as_2dim_table_float(YUV_1, height)[3][line];
		as_2dim_table_float(X_Sym_Col, height)[2][line]
		        = as_2dim_table_float(YUV_1, height)[2][line];
		as_2dim_table_float(X_Sym_Col, height)[3][line]
		        = as_2dim_table_float(YUV_1, height)[1][line];

		as_2dim_table_float(X_Sym_Col, height)[width+4][line]
		        = as_2dim_table_float(YUV_1, height)[width-2][line];
		as_2dim_table_float(X_Sym_Col, height)[width+5][line]
		        = as_2dim_table_float(YUV_1, height)[width-3][line];
		as_2dim_table_float(X_Sym_Col, height)[width+6][line]
		        = as_2dim_table_float(YUV_1, height)[width-4][line];
		as_2dim_table_float(X_Sym_Col, height)[width+7][line]
		        = as_2dim_table_float(YUV_1, height)[width-5][line];
	}

	/* Fil in other cases */
	for (int col=0; col<width; col++) {
		for (int line=0; line<height; line++) {
			as_2dim_table_float(X_Sym_Col, height)[col+4][line] = as_2dim_table_float(YUV_1, height)[col][line];
		}
	}
} 

void rearrange_3(float *X_1, unsigned int width, unsigned int height, float *X_Sym_Line){

		for (int col=0; col<(width); col++) {
		as_2dim_table_float(X_Sym_Line, height+8)[col][0]
		        = as_2dim_table_float(X_1, height)[col][4];
		as_2dim_table_float(X_Sym_Line, height+8)[col][1]
		        = as_2dim_table_float(X_1, height)[col][3];
		as_2dim_table_float(X_Sym_Line, height+8)[col][2]
		        = as_2dim_table_float(X_1, height)[col][2];
		as_2dim_table_float(X_Sym_Line, height+8)[col][3]
		        = as_2dim_table_float(X_1, height)[col][1];

		as_2dim_table_float(X_Sym_Line, height+8)[col][height+8-4]
		        = as_2dim_table_float(X_1, height)[col][height-2];
		as_2dim_table_float(X_Sym_Line, height+8)[col][height+8-3]
		        = as_2dim_table_float(X_1, height)[col][height-3];
		as_2dim_table_float(X_Sym_Line, height+8)[col][height+8-2]
		        = as_2dim_table_float(X_1, height)[col][height-4];
		as_2dim_table_float(X_Sym_Line, height+8)[col][height+8-1]
		        = as_2dim_table_float(X_1, height)[col][height-5];
	
	}
	for (int col=0; col<width; col++) {
		for (int line=0; line<height; line++) {
			as_2dim_table_float(X_Sym_Line, height+8)[col][line+4]
			           = as_2dim_table_float(X_1, height)[col][line];
		}
	}
}


/****************************************************/
/****************** Main function *******************/
/****************************************************/
/*int main(void)
{
	fprintf(stdout,"CNES Image Processing Algorithm\n");
	unsigned char *image = load_img("imagette.bin");

	demo_malvar(image, 1024, 1024);

	free(image);

  	/*float *G = calloc(300, sizeof(float));
  	as_2dim_table_float(G, 20)[0][0] = 8;
	as_2dim_table_float(G, 20)[1][0] = 5;
	as_2dim_table_float(G, 20)[0][6] = 9;
	as_2dim_table_float(G, 20)[8][3] = 3;
	as_2dim_table_float(G, 20)[7][7] = 10;
	as_2dim_table_float(G, 20)[3][4] = 4;

	print_matrix_TL(G, 20);*/

	//_sleep(1000000);
	/*return EXIT_SUCCESS;
}*/
