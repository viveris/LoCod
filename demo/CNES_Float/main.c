#include <math.h>
#ifndef LOCOD_FPGA
#include "locod.h"

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

#define WID 32
#define HGT 32
#define DATA_SIZE_TOTAL (WID * HGT)
#define DATA_SIZE_MAX 1048576 /* 1 MB  (W = 1024 * H = 1024)  */ 
#define DATA_S_MIN_4_EACH ((WID - 4) * (HGT - 4)) 
#define DATA_S_W4PLUS_H4MIN ((WID + 4) * (HGT - 4)) 

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
	float Red[DATA_SIZE_TOTAL];
	float Green[DATA_SIZE_TOTAL];
	float Blue[DATA_SIZE_TOTAL];	
};
struct malvar_result malvar_result= { 0 };

struct RGB_data {
	float R[DATA_S_MIN_4_EACH];
	float G[DATA_S_MIN_4_EACH];
	float B[DATA_S_MIN_4_EACH];	
};
struct RGB_data ansc_out = {0};

struct YUV_data {
	float YUV_1[DATA_S_MIN_4_EACH];
	float YUV_2[DATA_S_MIN_4_EACH];
	float YUV_3[DATA_S_MIN_4_EACH];	
};
struct YUV_data YUV = {0};

/*struct filt_data {
	float tmp_LP[1020][510];
	float tmp_HP[1020][510];
};

struct filt_data_2 {
	float tmp_LP[520200];
};*/

unsigned int width = WID;
unsigned int height = HGT;


float acc_one_out[DATA_S_W4PLUS_H4MIN] = {0};
//struct filt_data_2 acc_one_out = {0};
float acc_two_out[1040400] = {0};

struct test{
	float X_Sym_Col[DATA_S_W4PLUS_H4MIN];
};

struct test tester = {0};
//float X_Sym_Line[1048560] = {0};

void acc_1(struct test* param, float *result) //filt_1 and rearrange_2
{
	int i = 0;	
	/*float LP_Filter[9] = {	1,
							1,
							1,
							1,
							1,
							1,
							1,
							1,
							1};

	float HP_Filter[7] = {1,
							1,
							1,
							1,
							1,
							1,
							1};  */


	//as_2dim_table_float(result, 1020)[0][0] = 151.0;
	/*for (i ; i< DATA_S_W4PLUS_H4MIN; i++){
		result[i] = (param->X_Sym_Col[i]) + 1;
	}*/
	//result[0] = (param->X_Sym_Col[0]) + 1;
	//result[1] = (param->X_Sym_Col[1]) + 1;
	result[2] = (param->X_Sym_Col[2]) + 1;
	//result[3] = (param->X_Sym_Col[3]) + 1;
	//result[4] = (param->X_Sym_Col[4]) + 1;
	result[5] = (param->X_Sym_Col[5]) + 1;

	/*int col, line;
		for (col=4; col<1020+3; col += 2) {
		for (line=1; line<1020; line++) {
			unsigned int ind = (col+1-4)/2;*/
			
			//as_2dim_table_float(result, 1020)[ind][line] = 1.0;
			       /* as_2dim_table_float(param, 1020)[col-4][line]*LP_Filter[0] +
			        as_2dim_table_float(param, 1020)[col-3][line]*LP_Filter[1] +
			        as_2dim_table_float(param, 1020)[col-2][line]*LP_Filter[2] +
			        as_2dim_table_float(param, 1020)[col-1][line]*LP_Filter[3] +
			        as_2dim_table_float(param, 1020)[col][line]*  LP_Filter[4] +
			        as_2dim_table_float(param, 1020)[col+1][line]*LP_Filter[5] +
			        as_2dim_table_float(param, 1020)[col+2][line]*LP_Filter[6] +
			        as_2dim_table_float(param, 1020)[col+3][line]*LP_Filter[7] +
			        as_2dim_table_float(param, 1020)[col+4][line]*LP_Filter[8];*/

			//as_2dim_table_float(result->tmp_HP, 1020)[ind][line] = 2
			//result->tmp_HP[line][ind] = 2;
			       /* as_2dim_table_float(param, 1020)[col+1-3][line]*HP_Filter[0] +
			        as_2dim_table_float(param, 1020)[col+1-2][line]*HP_Filter[1] +
			        as_2dim_table_float(param, 1020)[col+1-1][line]*HP_Filter[2] +
			        as_2dim_table_float(param, 1020)[col+1][line]  *HP_Filter[3] +
			        as_2dim_table_float(param, 1020)[col+1+1][line]*HP_Filter[4] +
			        as_2dim_table_float(param, 1020)[col+1+2][line]*HP_Filter[5] +
			        as_2dim_table_float(param, 1020)[col+1+3][line]*HP_Filter[6];*/
		//}
	//}
}



/*void acc_2(float *param, float *result) //filt_2 and rearrange_4
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
}*/



/*void acc_3(struct RGB_data *param, float *result)
{
	
}*/
	
/*void acc_4(struct malvar_input *param, struct malvar_result *result)
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
	init_dma();
   	FILE *result_file;
	FILE *result_file1;
	FILE *result_file2;
	struct data data = { 0 };
	ctx.buff = &data;
	
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
			RGB_to_YUV(as_2dim_table_float(ansc_out.G, height-4)[col][line],
			           as_2dim_table_float(ansc_out.R, height-4)[col][line],
			           as_2dim_table_float(ansc_out.B, height-4)[col][line],
			           &as_2dim_table_float(YUV.YUV_1, height-4)[col][line],
			           &as_2dim_table_float(YUV.YUV_2, height-4)[col][line],
			           &as_2dim_table_float(YUV.YUV_3, height-4)[col][line]);
		}
	}
	fprintf(stdout, "after RGB to YUV\n");
	result_file = fopen("YUV_1.bin", "wb+");
	fwrite(YUV.YUV_1, sizeof(float), DATA_S_MIN_4_EACH, result_file);
	fclose(result_file);
	fprintf(stdout, ".bin file of YUV_1 written\n");

	rearrange_1(YUV.YUV_1, width-4, height-4,tester.X_Sym_Col);

	result_file1 = fopen("X_Sym_Col.bin", "wb+");
	fwrite(tester.X_Sym_Col, sizeof(float), DATA_S_W4PLUS_H4MIN, result_file1);
	fclose(result_file1);
	fprintf(stdout, ".bin file of X_Sym_Col written\n");
	/*fprintf(stdout,"X_sym_col \n");
	for(int g = 0; g < DATA_S_W4PLUS_H4MIN; g++){
		if (g % 68 == 0){
			fprintf(stdout,"\n");
		}
		fprintf(stdout,"[%d] : %f", g, tester.X_Sym_Col[g]);
	}*/

	struct fpga_param acc_1_in = {0};
	acc_1_in.p = &tester;
	acc_1_in.len = sizeof(struct test);//1048560 * sizeof(float);

	struct fpga_param acc_1_out = {0};
	acc_1_out.p = acc_one_out;
	acc_1_out.len = DATA_S_W4PLUS_H4MIN * sizeof(float);

/*	struct fpga_param acc_2_out = {0};
	acc_2_out.p = acc_two_out;
	acc_2_out.len = sizeof(acc_two_out);*/

	fprintf(stdout,"CNES Image Processing Algorithm\n");

	FPGA(acc_1, acc_1_in, acc_1_out, 0);
	wait_accelerator(&acc_1_out, 0);

	result_file = fopen("ACC_one_out.bin", "wb+");
	fwrite(acc_one_out, sizeof(float), DATA_S_W4PLUS_H4MIN, result_file);
	fclose(result_file);
	fprintf(stdout, ".bin file of ACC_1_out written\n");

	for (int y = 0; y < 16; y++){
		fprintf(stdout,"Xsym[0][%d]: %f ",y, as_2dim_table_float(tester.X_Sym_Col, height-4)[y][0]);
		fprintf(stdout,"Acc1[0][%d]: %f \n",y, as_2dim_table_float(acc_one_out, height-4)[y][0]);
	}
	fprintf(stdout, "\n");
	fprintf(stdout,"size of W4plus_Hmin4 : %d",DATA_S_W4PLUS_H4MIN);
	//fprintf(stdout,"size of test %i \n", sizeof(struct test));

//	rearrange_3(acc_one_out, width-4, height-4, X_Sym_Line);
	/*FPGA(acc_2, acc_1_out, acc_2_out, 1);
	wait_accelerator(&acc_2_out, 1);*/
	/*result_file = fopen("tmp_LP1.bin", "wb+");
	fwrite(&acc_one_out, sizeof(float), 1020*510, result_file);
	fclose(result_file);
	fprintf(stdout, ".bin file of tmp_LP1 written\n");*/
		
	/*for(int l = 0;l < 10; l++){
		fprintf(stdout,"x_sym_col [0][%i]: %f \n", l, (as_2dim_table_float(&tester.X_Sym_Col, 1020)[0][l]));
	}
	for(int k = 0;k < 10; k++){
		fprintf(stdout,"acc_1_out [0][%i]: %f \n", k, (as_2dim_table_float(&acc_one_out, 1020)[0][k]));
	}*/
	/*result_file2 = fopen("tmp_HP1.bin", "wb+");
	fwrite(acc_one_out.tmp_HP, sizeof(float), 1020*510, result_file2);
	fclose(result_file2);
	fprintf(stdout, ".bin file of tmp_HP1 written\n");*/









	/*struct fpga_param image = { 0 };
	image.p = malvar_input.picture_file;
	image.len = DATA_SIZE_MAX;

	struct fpga_param malvar_res = { 0 };
	malvar_res.p = malv_result;
	malvar_res.len = sizeof(malvar_result);//12582944/*((4 * sizeof(malv_result)) + 3 * DATA_SIZE_MAX * sizeof(float))*/;



    

	//result_file_1 = fopen("acc_1_R_out.bin", "wb+");
	//fwrite(malv_result->R, sizeof(float), 1024*1024, result_file_1);
	//fclose(result_file_1); 

	//FPGA(acc_4, image, malvar_res, 3);
	//wait_accelerator(&malvar_res, 3);
	//fprintf(stdout,"image [1][1] : %i\n", as_2dim_table_uchar(ctx.buff->data, 1024)[1][1]);
	//fprintf(stdout,"image_f [1][1] : %f\n", as_2dim_table_float(malvar_input.picture_file, 1024)[1][1]);
	/*fprintf(stdout,"malvar_res->R [1][1] : %f\n", as_2dim_table_float(malvar_result.R, 1024)[1][1]);
	fprintf(stdout,"malvar_res->G [1][1] : %f\n", as_2dim_table_float(malvar_result.G, 1024)[1][1]);
	fprintf(stdout,"malvar_res->B [1][1] : %f\n", as_2dim_table_float(malvar_result.B, 1024)[1][1]);*/
	//fprintf(stdout, "malvar_result.Red[4][4]: %f\n", as_2dim_table_float(malv_result->R, 1024)[4][4]/*malvar_result.Red[8]*/ );

/*	result_file = fopen("acc_4_out.bin", "wb+");
	fwrite(malvar_result.Red, sizeof(float), DATA_SIZE_MAX, result_file);
	fclose(result_file);
	fprintf(stdout, ".bin file of malvar_result->R written\n");
	fprintf(stdout, "end\n");*/

	//FPGA(acc_2, chan_YUV_fpga, X_sym_col_res, 1);
//	print_matrix_TL(malv_result->R, 1020);
//	demo_malvar(ctx.buff->data, 1024, 1024);


	
	
}
#endif
