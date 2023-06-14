#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <math.h>
#include <string.h>
#include <time.h>


void print_matrix_TL(float *matrix, unsigned int height);
void print_matrix_TR(float *matrix, unsigned int height);
void print_matrix_BL(float *matrix, unsigned int height);
void print_matrix_BR(float *matrix, unsigned int height);

float interpolation_Malvar(unsigned char *matrix,
                           float kernel[5][5],
                           unsigned int line_start,
                           unsigned int col_start, 
                           unsigned int height);

float anscombe_trf(float pixel, float ca, float cb);

unsigned char *malvar_anscombe( unsigned char *bayer_img, unsigned int width, 
                                unsigned int height, float *R_cut, float *G_cut, 
                                float *B_cut);

void rearrange_1(float *YUV_1, unsigned int width, unsigned int height, float *X_Sym_Col);
void rearrange_3(float *X_1, unsigned int width, unsigned int height, float *X_Sym_Line);

float *RGB_to_YUV(float R,
                   float G,
                   float B,
                   float *YUV_1,
                   float *YUV_2,
                   float *YUV_3);

void DWT_CCSDS_single_level(float *chan_YUV, unsigned int height, unsigned int width);

//unsigned char *demo_malvar(unsigned char *bayer_img, unsigned int width, unsigned int height);