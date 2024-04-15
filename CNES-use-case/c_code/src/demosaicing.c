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
#include <unistd.h>
#include <string.h>
#include <math.h>


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
}


/****************************************************/
/**************** Utility functions *****************/
/****************************************************/
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


int demo_malvar(unsigned char *bayer_img, unsigned int width, unsigned int height, char *output_malvar_file, char *output_filter_file)
{
	int ret;									                /* Return value */
	unsigned char *buffer;						                /* In and out buffer */
	buffer = malloc(3*width*height*sizeof(unsigned char));
	
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

	/* Save Malvar output */
    /* Format [R0, G0, B0, R1, G1, B1, ... ] */
    for (unsigned int line=0; line<height; line++) {						/* Image line */
        for (unsigned int col=0; col<width; col++) {						/* Image column */
            buffer[3*(line*width + col) + 0] = (unsigned char)as_2dim_table_double(R, height)[line][col];
            buffer[3*(line*width + col) + 1] = (unsigned char)as_2dim_table_double(G, height)[line][col];
            buffer[3*(line*width + col) + 2] = (unsigned char)as_2dim_table_double(B, height)[line][col];
        }
    }
    ret = save_byte_data(output_malvar_file, buffer, 3*height*width);
    if (ret != 0) {
        printf("Problem openning output malvar file\n");
        return ret;
    }


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

	DWT_CCSDS_single_level(YUV_1, height-4, width-4);
//	DWT_CCSDS_single_level(YUV_2, height-4, width-4);
//	DWT_CCSDS_single_level(YUV_3, height-4, width-4);

	/* Save final filtered YUV_1 output */
    for (unsigned int line=0; line<height-4; line++) {						/* Image line */
        for (unsigned int col=0; col<width-4; col++) {						/* Image column */
            buffer[line*(width-4) + col] = (unsigned char)as_2dim_table_double(YUV_1,height-4)[line][col];
        }
    }
    ret = save_byte_data(output_filter_file, buffer, (height-4)*(width-4));
    if (ret != 0) {
        printf("Problem openning output image file\n");
        return ret;
    }

	double *Y_BF = calloc(513*513, sizeof(double));
	for(int row = 0; row < 513; row++){
		for(int col = 0; col < 513; col++){
			as_2dim_table_double(Y_BF,513)[col][row] = as_2dim_table_double(YUV_1,height-4)[col][row];
		}
	}

	free(buffer);

	return 0;
}


/****************************************************/
/****************** Main function *******************/
/****************************************************/
int main(int argc, char *argv[])
{
	/* Variables */
    int ret;									                /* Return value */
    int opt;									                /* User option */
	char *input_image_file;						                /* Input image file */
    char *output_malvar_file = "output_malvar_ccode.bin";		/* Output Malvar file */
    char *output_filter_file = "output_filter_ccode.bin";		/* Output Filter file */
	unsigned char *imagette;									/* Input data */

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
    imagette = malloc(1024*1024*sizeof(unsigned char));

	/* Open image */
    ret = load_byte_data(input_image_file, imagette, 1024*1024);
    if (ret != 0) {
        printf("Problem openning input image file\n");
        return ret;
    }

	demo_malvar(imagette, 1024, 1024, output_malvar_file, output_filter_file);

	free(imagette);

	return 0;
}
