#include "map_memory.h"

int main(void)
{
	if ( init_dma() ) {
		fprintf(stdout, "Error on DMA initialization\n");
		return EXIT_FAILURE;
	}

	set_in_1(0x87);
	set_in_2(0x64);
	set_out_1(0x0);

	int *mem_dma = NULL;
	if ( map_phys_addr(MEM_DMA_BASE, sizeof(int)*12, (void **)&mem_dma) ) {
		fprintf(stdout, "Error of DMA mapping\n");
		return EXIT_FAILURE;
	}

	mem_dma[0] = 0x00345678;
	mem_dma[1] = 0x04345678;
	mem_dma[2] = 0x08345678;
	mem_dma[3] = 0x0c345678;
	mem_dma[4] = 0x10345678;
	mem_dma[5] = 0x14345678;
	mem_dma[6] = 0x18345678;
	mem_dma[7] = 0x1c345678;
	mem_dma[8] = 0x20345678;
	mem_dma[9] = 0x24345678;
	mem_dma[10] = 0x28345678;
	mem_dma[11] = 0x2c345678;

	if ( start_process() ) {
		fprintf(stdout, "Error on starting FPGA processing\n");
		return EXIT_FAILURE;
	}

	if ( wait_process() ) {
		fprintf(stdout, "Error on waiting FPGA processing\n");
		return EXIT_FAILURE;
	}

	fprintf(stdout, "Output after processing:\n");
	for (int i=0; i<12; i++) {
		fprintf(stdout, "0x%X\n", mem_dma[i]);
	}

	return EXIT_SUCCESS;
}
