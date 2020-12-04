#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <math.h>
#include <string.h>

#define as_2dim_table_uchar(ptr, height) ((unsigned char (*)[height])ptr)
#define as_2dim_table_float(ptr, height) ((float (*)[height])ptr)
#define as_2dim_table_double(ptr, height) ((double (*)[height])ptr)

void print_matrix(double *matrix, unsigned int height)
{
	for (int i=0; i<10; i++) {
		for (int j=0; j<10; j++) {
			fprintf(stdout, "%.3lf\t", as_2dim_table_double(matrix, height)[j][i]);
		}
		fprintf(stdout, "\n");
	}
}

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
			        as_2dim_table_double(X_sym_col, height)[col][line]*LP_Filter[4] +
			        as_2dim_table_double(X_sym_col, height)[col+1][line]*LP_Filter[5] +
			        as_2dim_table_double(X_sym_col, height)[col+2][line]*LP_Filter[6] +
			        as_2dim_table_double(X_sym_col, height)[col+3][line]*LP_Filter[7] +
			        as_2dim_table_double(X_sym_col, height)[col+4][line]*LP_Filter[8];

			as_2dim_table_double(tmp_HP1, height)[ind][line] =
			        as_2dim_table_double(X_sym_col, height)[col+1-3][line]*HP_Filter[0] +
			        as_2dim_table_double(X_sym_col, height)[col+1-2][line]*HP_Filter[1] +
			        as_2dim_table_double(X_sym_col, height)[col+1-1][line]*HP_Filter[2] +
			        as_2dim_table_double(X_sym_col, height)[col+1][line]*HP_Filter[3] +
			        as_2dim_table_double(X_sym_col, height)[col+1+1][line]*HP_Filter[4] +
			        as_2dim_table_double(X_sym_col, height)[col+1+2][line]*HP_Filter[5] +
			        as_2dim_table_double(X_sym_col, height)[col+1+3][line]*HP_Filter[6];
		}
	}

	double *X_sym_line = calloc((height+8)*width, sizeof(double));
	/* Compute symetric on 4 first and end lines */
	for (int col=0; col<width; col++) {
		as_2dim_table_double(X_sym_line, height+8)[col][0]
		        = as_2dim_table_double(chan_YUV, height)[col][4];
		as_2dim_table_double(X_sym_line, height+8)[col][1]
		        = as_2dim_table_double(chan_YUV, height)[col][3];
		as_2dim_table_double(X_sym_line, height+8)[col][2]
		        = as_2dim_table_double(chan_YUV, height)[col][2];
		as_2dim_table_double(X_sym_line, height+8)[col][3]
		        = as_2dim_table_double(chan_YUV, height)[col][1];

		as_2dim_table_double(X_sym_line, height+8)[col][height+4]
		        = as_2dim_table_double(chan_YUV, height)[col][height-2];
		as_2dim_table_double(X_sym_line, height+8)[col][height+5]
		        = as_2dim_table_double(chan_YUV, height)[col][height-3];
		as_2dim_table_double(X_sym_line, height+8)[col][height+6]
		        = as_2dim_table_double(chan_YUV, height)[col][height-4];
		as_2dim_table_double(X_sym_line, height+8)[col][height+7]
		        = as_2dim_table_double(chan_YUV, height)[col][height-5];
	}

	/* Fil in other cases */
	for (int col=0; col<width; col++) {
		for (int line=0; line<height; line++) {
			as_2dim_table_double(X_sym_line, height)[col][line+4]
			           = as_2dim_table_double(chan_YUV, height)[col][line];
		}
	}

	fprintf(stdout, "%s Sym :\n", __func__);
	print_matrix(X_sym_line, height);


	double *tmp_LP2 = calloc(height/2*width, sizeof(double));
	double *tmp_HP2 = calloc(height/2*width, sizeof(double));

	fprintf(stdout, "%s LP1 :\n", __func__);
	print_matrix(tmp_LP1, height);
}

 unsigned char *demo_malvar(unsigned char *bayer_img, unsigned int width, unsigned int height)
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
	for (unsigned int line=0; line<(height - 2); line = line + 2) {
		for (unsigned int col=0; col<(width - 2); col = col + 2) {
			as_2dim_table_double(G, 1024)[col+1][line] = as_2dim_table_uchar(bayer_img, 1024)[col+1][line];
			as_2dim_table_double(G, 1024)[col][line+1] = as_2dim_table_uchar(bayer_img, 1024)[col][line+1];
			as_2dim_table_double(R, 1024)[col][line] = as_2dim_table_uchar(bayer_img, 1024)[col][line];
			as_2dim_table_double(B, 1024)[col+1][line+1] = as_2dim_table_uchar(bayer_img, 1024)[col+1][line+1];

			/* Alow to skip the edges of the image (2x2) */
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

	double *R_cut = calloc((width-4)*(height-4), sizeof(double));
	double *G_cut = calloc((width-4)*(height-4), sizeof(double));
	double *B_cut = calloc((width-4)*(height-4), sizeof(double));

	/* Anscombe */
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

	fprintf(stdout, "R :\n");
	print_matrix(R_cut, height-4);

	fprintf(stdout, "G :\n");
	print_matrix(G_cut, height-4);

	fprintf(stdout, "B :\n");
	print_matrix(B_cut, height-4);


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

	fprintf(stdout, "YUV 1 :\n");
	print_matrix(YUV_1, height-4);

	fprintf(stdout, "YUV 2 :\n");
	print_matrix(YUV_2, height-4);

	fprintf(stdout, "YUV 3 :\n");
	print_matrix(YUV_3, height-4);

	DWT_CCSDS_single_level(YUV_1, height-4, width-4);

	return NULL;
}



/****************************************************/
/**************** Utility functions *****************/
/****************************************************/

unsigned char *load_img(const char *path)
{
	FILE *ptr;

	/* Open file */
	ptr = fopen(path,"rb");
	if (ptr == NULL) {
		perror("Cannot open file");
	}

	/* Get filesize */
	fseek(ptr, 0L, SEEK_END);
	unsigned int fsize = ftell(ptr);
	/* Get back to the start of file */
	fseek(ptr, 0L, SEEK_SET);

	/* Allocate memory to load file data */
	unsigned char *file_data = malloc(fsize);

	/* Load into memory and close file */
	fread(file_data, fsize, 1, ptr);
	fclose(ptr);

	fprintf(stdout, "File %s opened, size=%u bytes\n", path, fsize);

	return file_data;
}


/****************************************************/
/****************** Main function *******************/
/****************************************************/
int main(void)
{
	unsigned char *image = load_img("imagette.bin");

	demo_malvar(image, 1024, 1024);

	free(image);

	return EXIT_SUCCESS;
}
