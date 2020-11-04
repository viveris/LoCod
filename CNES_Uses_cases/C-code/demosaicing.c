#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#define as_2dim_table_uchar(ptr, height) ((unsigned char (*)[height])ptr)
#define as_2dim_table_float(ptr, height) ((float (*)[height])ptr)

void print_matrix(float *matrix)
{
	for (int i=0; i<10; i++) {
		for (int j=0; j<10; j++) {
			fprintf(stdout, "%.3f\t", as_2dim_table_float(matrix, 1024)[j][i]);
		}
		fprintf(stdout, "\n");
	}
}

/****************************************************/
/************** Math util functions *****************/
/****************************************************/
float interpolation_Malvar(unsigned char *matrix,
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

/****************************************************/
/***************** Algo functions *******************/
/****************************************************/
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


	/* Green 1 */
	float *G = calloc(width*height, sizeof(float));
	float *R = calloc(width*height, sizeof(float));
	float *B = calloc(width*height, sizeof(float));
	for (unsigned int line=0; line<(height - 2); line = line + 2) {
		for (unsigned int col=0; col<(width - 2); col = col + 2) {
			as_2dim_table_float(G, 1024)[col+1][line] = as_2dim_table_uchar(bayer_img, 1024)[col+1][line];
			as_2dim_table_float(G, 1024)[col][line+1] = as_2dim_table_uchar(bayer_img, 1024)[col][line+1];
			as_2dim_table_float(R, 1024)[col][line] = as_2dim_table_uchar(bayer_img, 1024)[col][line];
			as_2dim_table_float(B, 1024)[col+1][line+1] = as_2dim_table_uchar(bayer_img, 1024)[col+1][line+1];

			/* Alow to skip the edges of the image (2x2) */
			if (line >= 2 && line <= height-3 && col >= 2 && col <= width-3) {
				/* Green channel */
				as_2dim_table_float(G, 1024)[col][line] = interpolation_Malvar(bayer_img, G_at_RB, line-2, col-2);
				as_2dim_table_float(G, 1024)[col+1][line+1] = interpolation_Malvar(bayer_img, G_at_RB, line-1, col-1);

				/* Red channel */
				as_2dim_table_float(R, 1024)[col+1][line] = interpolation_Malvar(bayer_img, R1, line-2, col-1);
				as_2dim_table_float(R, 1024)[col][line+1] = interpolation_Malvar(bayer_img, R2, line-1, col-2);
				as_2dim_table_float(R, 1024)[col+1][line+1] = interpolation_Malvar(bayer_img, R3, line-1, col-1);

				/* Blue channel */
				as_2dim_table_float(B, 1024)[col][line] = interpolation_Malvar(bayer_img, B3, line-2, col-2);
				as_2dim_table_float(B, 1024)[col+1][line] = interpolation_Malvar(bayer_img, B2, line-2, col-1);
				as_2dim_table_float(B, 1024)[col][line+1] = interpolation_Malvar(bayer_img, B1, line-1, col-2);
			}
		}
	}

	fprintf(stdout, "G :\n");
	print_matrix(G);

	fprintf(stdout, "R :\n");
	print_matrix(R);

	fprintf(stdout, "B :\n");
	print_matrix(B);

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
	unsigned char *image = load_img("/home/hugo/Projets/LoCod/Worskspace/PandA/locod/CNES_Uses_cases/imagette.bin");

	demo_malvar(image, 1024, 1024);

	free(image);

	return EXIT_SUCCESS;
}
