#ifndef LOCOD_FPGA
#include "locod.h"
#include <math.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#endif /* LOCOD_FPGA */

/* Struct to store image and size of the image */
#define DATA_SIZE_MAX 262144 /* 256 kB */
struct data {
	unsigned int len;
	float data[DATA_SIZE_MAX];
};


#ifndef LOCOD_FPGA
/* Internal context used to store global variables of the program */
struct private_context {
	char *picture_file;
	struct data *buff;
};

struct private_context ctx = { 0 };

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

	if (file_size > (DATA_SIZE_MAX*4)) {
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

	ctx.buff->len = file_size / sizeof(float);

	fclose(file);
	return 0;

close_file:
	fclose(file);
out:
	return -1;
}
#endif /* LOCOD_FPGA */

void acc_1(struct data* param, float *result)
{
	int i;

	for (i = 0; i<param->len; i++) {
		result[i] = param->data[i] + 1.0;
	}

}

void pic_multiplication(struct data* param, float *result)
{
	int i;

	for (i = 0; i<param->len; i++) {
		result[i] = param->data[i] * 2;
	}
}

#ifndef LOCOD_FPGA
int main(int argc, char **argv)
{
	init_locod(1);
	int b; /* TODO to be removed when only two param for interface */
	float *result = NULL;
	FILE *result_file;
	struct data data = { 0 };
	ctx.buff = &data;
	int width = 0;
	int comp = 0;
	//static int filed = -1;

	fprintf(stdout, "data addr = %p\n", ctx.buff);

	handle_arg(argc, argv);

	if (load_file(ctx.picture_file)) {
		exit(EXIT_FAILURE);
	}

	/* Allocate buffer for result */
	result = malloc(ctx.buff->len * sizeof(int));

	FPGA(acc_1, ctx.buff, result, 0);
	wait_accelerator(result, 0);

	/* Write FPGA result into a file */
	result_file = fopen("result.bin", "wb");
	if (!result_file) {
		perror("Cannot open file result.bin");
		goto free_and_failure;
	}

	if (fwrite(result, 1, ctx.buff->len*sizeof(int), result_file) != ctx.buff->len*sizeof(int)) {
		fprintf(stderr, "Cannot write file result.bin\n");
		goto free_and_failure;
	}
	switch(ctx.buff->len){

		case 256 :
			width = 16;
			fprintf(stdout,"ctx.buff->len = %d, Width = %d \n", ctx.buff->len, width);
			break;
		case 324 :	
			width = 18;
			fprintf(stdout,"ctx.buff->len = %d, Width = %d \n", ctx.buff->len, width);
			break;
		case 400 :
			width = 20;
			fprintf(stdout,"ctx.buff->len = %d, Width = %d \n", ctx.buff->len, width);
			break;
		case 484 :	
			width = 22;
			fprintf(stdout,"ctx.buff->len = %d, Width = %d \n", ctx.buff->len, width);
			break;
		case 529 :	
			width = 23;
			fprintf(stdout,"ctx.buff->len = %d, Width = %d \n", ctx.buff->len, width);
			break;	
		case 576 :
			width = 24;
			fprintf(stdout,"ctx.buff->len = %d, Width = %d \n", ctx.buff->len, width);
			break;
		case 676 :	
			width = 26;
			fprintf(stdout,"ctx.buff->len = %d, Width = %d \n", ctx.buff->len, width);
			break;
		case 784 :
			width = 28;
			fprintf(stdout,"ctx.buff->len = %d, Width = %d \n", ctx.buff->len, width);
			break;
		case 900 :	
			width = 30;
			fprintf(stdout,"ctx.buff->len = %d, Width = %d \n", ctx.buff->len, width);
			break;
		case 1024 :
			width = 32;
			fprintf(stdout,"ctx.buff->len = %d, Width = %d \n", ctx.buff->len, width);
			break;
		case 4096 :	
			width = 64;
			fprintf(stdout,"ctx.buff->len = %d, Width = %d \n", ctx.buff->len, width);
			break;
		case 16384 :
			width = 128;
			fprintf(stdout,"ctx.buff->len = %d, Width = %d \n", ctx.buff->len, width);
			break;
		case 65536 :
			width = 256;
			fprintf(stdout,"ctx.buff->len = %d, Width = %d \n", ctx.buff->len, width);
			break;
		case 262144 :	
			width = 512;
			fprintf(stdout,"ctx.buff->len = %d, Width = %d \n", ctx.buff->len, width);
			break;
		case 1048576 :
			width = 1024;
			fprintf(stdout,"ctx.buff->len = %d, Width = %d \n", ctx.buff->len, width);
			break;	
	}
	/*fprintf(stdout, "\nInput Data :\n");
	for (int j = 0; j < ctx.buff->len; j++){
		
		if (j % width == 0){
			fprintf(stdout," \n");	
		}
		fprintf(stdout, "%0.1f \t", ctx.buff->data[j]);
	}
	fprintf(stdout, "\n\nResult Data :\n");
	for (int j = 0; j < ctx.buff->len; j++){
		if (j % width == 0){
			fprintf(stdout," \n");	
		}
		fprintf(stdout, "%0.1f \t", *(result + j));
		
	}*/
	fprintf(stdout, "\n");
	for(int h = 0; h < ctx.buff->len; h++){
		if((*(result + h) - ctx.buff->data[h])==1){
			comp++;
		}
	}
	fprintf(stdout, "Good cells : %d out of %d\n", comp, ctx.buff->len);
	//fprintf(stdout, "First cell : %d, First cell +1 : %d, First cell +2 : %d \n", *result, *(result+1), *(result+2));
	//CPU(pic_multiplication, ctx.buff, result);
	/*fprintf(stdout, "memory dump of Input data on CPU RAM\n");
	dump_memory(ctx.buff->data, ctx.buff->len);
	fprintf(stdout, "\n");
	fprintf(stdout, "memory dump of Output data on CPU RAM\n");
	dump_memory(result, ctx.buff->len);
	fprintf(stdout, "\n");
	filed = open("/dev/mem", O_RDWR | O_SYNC);
	fprintf(stdout, "memory dump of Input data on FPGA RAM\n");
	dump_memory((void*) MEM_DMA_BASE, ctx.buff->len);
	fprintf(stdout, "\n");*/

	deinit_locod();

	exit(EXIT_SUCCESS);

free_and_failure:
	free(result);
	exit(EXIT_FAILURE);
}
#endif
