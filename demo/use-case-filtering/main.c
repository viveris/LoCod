#include <math.h>
#ifndef LOCOD_FPGA
#include "locod.h"
#include "demosaicing_float.h"

#include <time.h>


#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#endif

#define as_2dim_table_uchar(ptr, height) ((unsigned char (*)[height])ptr)
#define as_2dim_table_float(ptr, height) ((float (*)[height])ptr)
#define as_2dim_table_double(ptr, height) ((double (*)[height])ptr)

#define DATA_SIZE_MAX 1048576 /* 1 MB */ 

struct data {
	unsigned int len;
	unsigned char data[DATA_SIZE_MAX];
};

struct private_context {
	char *picture_file;
	struct data *buff;
};
struct private_context ctx = { 0 };

#ifndef LOCOD_FPGA

void handle_arg(int argc, char **argv)
{
	int c = 0;

	while ((c = getopt (argc, argv, "f:")) != -1)
		switch (c)
		{
			case 'f':
				ctx.picture_file = optarg;
				break;
			case '?':
				if (optopt == 'f') {
					fprintf (stderr, "Option -%c requires an argument.\n", optopt);
				}
				else if (isprint (optopt)) {
					fprintf (stderr, "Unknown option `-%c'.\n", optopt);
				}
				else {
					fprintf (stderr,
							"Unknown option character `\\x%x'.\n",
							optopt);
				}
			default:
				abort();
		}
}

int load_file(const char *file_path)
{
	FILE *file;
	unsigned long file_size;
	size_t nread = 0;

	file = fopen(file_path, "rb");
	if (!file) {
		perror("Cannot open input file");
		goto out;
	}

	/* Get file size */
	fseek(file, 0, SEEK_END);
	file_size = ftell(file);
	fseek(file, 0, SEEK_SET);

	if (file_size > DATA_SIZE_MAX) {
		fprintf(stderr, "File %s too big : file size %ld, max size %d\n",
		        file_path, file_size, DATA_SIZE_MAX);
		goto close_file;
	}

	fprintf(stdout, "Loading %ld bytes from file %s\n", file_size, file_path);
	nread = fread(ctx.buff->data, 1, file_size, file);
	if ( nread != file_size) {
		fprintf(stderr, "Cannot read all data from file (number of bytes read %zu)\n", nread);
		goto close_file;
	}

	ctx.buff->len = file_size / sizeof(char);

	fclose(file);
	return 0;

close_file:
	fclose(file);
out:
	return -1;
}
#endif /* LOCOD_FPGA */

struct malvar_result {
	float Red[DATA_SIZE_MAX];
	float Green[DATA_SIZE_MAX];
	float Blue[DATA_SIZE_MAX];	
};
struct malvar_result malvar_result= { 0 };


struct param_test {
	int a[2];
	int b[2];
};

/*req*/
struct RGB_data {
	float R[1040400];
	float G[1040400];
	float B[1040400];	
};
struct RGB_data ansc_out = {0};
/*req*/
struct YUV_data {
	float YUV_1[1040400];
	float YUV_2[1040400];
	float YUV_3[1040400];	
};
struct YUV_data YUV = {0};

struct filt_data_1 {
	float tmp_LP[520200];
	float tmp_HP[520200];
};

struct filt_data_2 {
	float tmp_HP[520200];
	float tmp_LP[520200];
};

unsigned int width = 1024;
unsigned int height = 1024;

//struct filt_data_1 acc_one_out = {0};
float acc_zero_out[1040400] = {0};
float acc_one_out[1040400] = {0};
float X_Sym_Col[1048560] = {0};
float X_Sym_Line[1048560] = {0};

void acc_0(float *param, float *result) //filt_1 and rearrange_2
{
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

	int col, line;
		for (col=4; col<1020+3; col += 2) {
			for (line=0; line<1020; line++) {
				unsigned int ind = (col+1-4)/2;
			
				as_2dim_table_float(result, 1020)[ind][line] = 
			        as_2dim_table_float(param, 1020)[col-4][line]*LP_Filter[0] +
			        as_2dim_table_float(param, 1020)[col-3][line]*LP_Filter[1] +
			        as_2dim_table_float(param, 1020)[col-2][line]*LP_Filter[2] +
			        as_2dim_table_float(param, 1020)[col-1][line]*LP_Filter[3] +
			        as_2dim_table_float(param, 1020)[col][line]*  LP_Filter[4] +
			        as_2dim_table_float(param, 1020)[col+1][line]*LP_Filter[5] +
			        as_2dim_table_float(param, 1020)[col+2][line]*LP_Filter[6] +
			        as_2dim_table_float(param, 1020)[col+3][line]*LP_Filter[7] +
			        as_2dim_table_float(param, 1020)[col+4][line]*LP_Filter[8];

				as_2dim_table_float(result, 1020)[ind+510][line] = 
			        as_2dim_table_float(param, 1020)[col+1-3][line]*HP_Filter[0] +
			        as_2dim_table_float(param, 1020)[col+1-2][line]*HP_Filter[1] +
			        as_2dim_table_float(param, 1020)[col+1-1][line]*HP_Filter[2] +
			        as_2dim_table_float(param, 1020)[col+1][line]  *HP_Filter[3] +
			        as_2dim_table_float(param, 1020)[col+1+1][line]*HP_Filter[4] +
			        as_2dim_table_float(param, 1020)[col+1+2][line]*HP_Filter[5] +
			        as_2dim_table_float(param, 1020)[col+1+3][line]*HP_Filter[6];
		}
	}
}



void acc_1(float *param, float *result) //filt_2 and rearrange_4
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
	int line,col;
	for (line=4; line<1020+3; line += 2) {
		unsigned int ind = (line+1-4)/2;
		for (col=0; col<1020; col++) {
			as_2dim_table_float(result, (1020))[col][ind] =
			        as_2dim_table_float(param, 1020+8)[col][line-4]*LP_Filter[0] +
			        as_2dim_table_float(param, 1020+8)[col][line-3]*LP_Filter[1] +
			        as_2dim_table_float(param, 1020+8)[col][line-2]*LP_Filter[2] +
			        as_2dim_table_float(param, 1020+8)[col][line-1]*LP_Filter[3] +
			        as_2dim_table_float(param, 1020+8)[col][line]  *LP_Filter[4] +
			        as_2dim_table_float(param, 1020+8)[col][line+1]*LP_Filter[5] +
			        as_2dim_table_float(param, 1020+8)[col][line+2]*LP_Filter[6] +
			        as_2dim_table_float(param, 1020+8)[col][line+3]*LP_Filter[7] +
			        as_2dim_table_float(param, 1020+8)[col][line+4]*LP_Filter[8];
			as_2dim_table_float(result, (1020))[col][ind+510] =
			        as_2dim_table_float(param, 1020+8)[col][line+1-3]*HP_Filter[0] +
			        as_2dim_table_float(param, 1020+8)[col][line+1-2]*HP_Filter[1] +
			        as_2dim_table_float(param, 1020+8)[col][line+1-1]*HP_Filter[2] +
			        as_2dim_table_float(param, 1020+8)[col][line+1]  *HP_Filter[3] +
			        as_2dim_table_float(param, 1020+8)[col][line+1+1]*HP_Filter[4] +
			        as_2dim_table_float(param, 1020+8)[col][line+1+2]*HP_Filter[5] +
			        as_2dim_table_float(param, 1020+8)[col][line+1+3]*HP_Filter[6];
		}
	}
}



/*void acc_3(struct float *param, float *result)
{
	
}*/
	
/*void acc_4(struct float *param, float *result)
{

}
void acc_5(struct data *param, int *result)
{
	
}
void acc_6(struct data *param, int *result)
{
	
}

void acc_7(struct float *param, float *result)
{
	
}*/

/*void acc_8(float *param, float *result)
{

}*/

#ifndef LOCOD_FPGA
int main(int argc, char **argv)
{
	init_accel_system(2);
   	FILE *result_file;
	FILE *result_file1;
	struct data data = { 0 };
	ctx.buff = &data;
	
	fprintf(stdout,"CNES Image Processing Algorithm - Demosaicing of a Bayer Image\n");
	fprintf(stdout, "data addr = %p\n", ctx.buff->data);
	handle_arg(argc, argv);
	if (load_file(ctx.picture_file)) {
		exit(EXIT_FAILURE);
	}

	malvar_anscombe(ctx.buff->data, width, height, ansc_out.R, ansc_out.G, ansc_out.B);
	
	/* Convert RGB to YUV */
	for (int line=0; line<width-4; line++) {
		/* Sum of column members */
		for (int col=0; col<height-4; col++) {
			/* TODO inverse R and G if CNES error */
			RGB_to_YUV(as_2dim_table_float(ansc_out.G, height-4)[col][line],
			           as_2dim_table_float(ansc_out.R, height-4)[col][line],
			           as_2dim_table_float(ansc_out.B, height-4)[col][line],
			           &as_2dim_table_float(YUV.YUV_1, height-4)[col][line],
			           &as_2dim_table_float(YUV.YUV_2, height-4)[col][line],
			           &as_2dim_table_float(YUV.YUV_3, height-4)[col][line]);
		}
	}
	rearrange_1(YUV.YUV_1, width-4, height-4, X_Sym_Col);

	struct fpga_param acc_0_in = {0};
	acc_0_in.p = X_Sym_Col;
	acc_0_in.len = sizeof(X_Sym_Col);

	struct fpga_param acc_0_out = {0};
	acc_0_out.p = acc_zero_out;
	acc_0_out.len = sizeof(acc_zero_out);

	struct fpga_param acc_1_in = {0};
	acc_1_in.p = X_Sym_Line;
	acc_1_in.len = sizeof(X_Sym_Line);
	
	struct fpga_param acc_1_out = {0};
	acc_1_out.p = acc_one_out;
	acc_1_out.len = sizeof(acc_one_out);

	FPGA(acc_0, acc_0_in, acc_0_out, 0);
	wait_accelerator(acc_0_out, 0);

	rearrange_3(acc_zero_out, width-4, height-4, X_Sym_Line);

	FPGA(acc_1, acc_1_in, acc_1_out, 1);
	wait_accelerator(acc_1_out, 1);

	deinit_accel_system();

	print_matrix_TL(acc_zero_out, 1020);
	print_matrix_TL(acc_one_out, 1020);

	result_file = fopen("acc_1_out.bin", "wb+");
	fwrite(acc_zero_out, sizeof(float), 1020*1020, result_file);
	fclose(result_file);
	fprintf(stdout, ".bin file of acc_zero_out written\n");
	result_file1 = fopen("acc_2_out.bin", "wb+");
	fwrite(acc_one_out, sizeof(float), 1020*1020, result_file1);
	fclose(result_file1);
	fprintf(stdout, ".bin file of acc_one_out written\n");
}
#endif
