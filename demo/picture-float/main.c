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
#include "locod.h"
#include <math.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#endif /* LOCOD_FPGA */


/* ========== Defines ========== */
#define IMAGE_WIDTH		1024
#define IMAGE_HEIGHT	1024
#define IMAGE_SIZE		IMAGE_WIDTH*IMAGE_HEIGHT


/* ========== Accelerator structures to store input and output data ========== */
struct input_image {
	float data[IMAGE_HEIGHT][IMAGE_WIDTH];
};

struct output_image {
	float data[IMAGE_HEIGHT][IMAGE_WIDTH];
};


/* ========== Accelerator functions ========== */
void acc_1(struct input_image *param, struct output_image *result) {
	for (int i=0; i<IMAGE_HEIGHT; i++) {						/* Image line */
		for (int j=0; j<IMAGE_WIDTH; j++) {						/* Image column */
			result->data[i][j] = param->data[i][j] + 3.7;
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

void print_image_TL(float image[IMAGE_HEIGHT][IMAGE_WIDTH]) {
	for (int i=0; i<10; i++) {					/* Image line */
		for (int j=0; j<10; j++) {				/* Image column */
			printf("%f\t", image[i][j]);
		}
		printf("\n");
	}
}


/* ========== Main function ========== */
int main(int argc, char *argv[]) {
	/* Variables */
	int ret;									/* Return value */
	int opt;									/* User option */
	char *input_image_file;						/* Input image file */
	char *output_image_file;					/* Output image file */
	unsigned char *buffer;						/* In and out buffer */
	struct input_image *input_image;			/* Accelerator input */
	struct output_image *output_image;			/* Accelerator output */

	/* Allocate in and out accelerator data structures */
	input_image = malloc(sizeof(struct input_image));
	output_image = malloc(sizeof(struct output_image));
	buffer = malloc(IMAGE_SIZE*sizeof(unsigned char));
	
	/* Parsing arguments */
	while ((opt = getopt(argc, argv, ":i:o:")) != -1) {
		switch (opt) {
			case 'i':
				input_image_file = optarg;
				break;
			case 'o':
				output_image_file = optarg;
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
				printf("Usage: %s [-f input_file]\n", argv[0]);
				return -1;
		}
	}

	/* Open image */
	ret = load_byte_data(input_image_file, buffer, IMAGE_SIZE);
	if (ret != 0) {
		printf("Problem openning input image file\n");
		return ret;
	}

	/* Place raw data in accelerator input structure */
	for (int i=0; i<IMAGE_HEIGHT; i++) {						/* Image line */
		for (int j=0; j<IMAGE_WIDTH; j++) {						/* Image column */
			input_image->data[i][j] = (float)buffer[i*IMAGE_WIDTH + j];
		}
	}

	/* LoCod initialization */
	init_locod(1);

	/* Exuecute acc_1 function in FPGA */
	FPGA(acc_1, input_image, output_image, 0);
	wait_accelerator(output_image, 0);

	/* LoCod deinitialization */
	deinit_locod();

	/* Print image result */
	print_image_TL(output_image->data);

	/* Place accelerator output into buffer */
	for (int i=0; i<IMAGE_HEIGHT; i++) {						/* Image line */
		for (int j=0; j<IMAGE_WIDTH; j++) {						/* Image column */
			buffer[i*IMAGE_WIDTH + j] = (unsigned char)output_image->data[i][j];
		}
	}

	/* Write FPGA result into a file */
	ret = save_byte_data(output_image_file, buffer, IMAGE_SIZE);
	if (ret != 0) {
		printf("Problem openning output image file\n");
		return ret;
	}

	/* Free memory */
	free(input_image);
	free(output_image);
	free(buffer);

	return 0;
}
#endif /* LOCOD_FPGA */
