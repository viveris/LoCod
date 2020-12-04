#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define FPGA_BASE_ADDR 0xA0000000

int main(void)
{
	int *ptr_fpga = NULL; /* Pointer to store virtual address pointing to fpga address */
	int fd = 0; /* File descriptor used to open /dev/mem device */

	/* Opening /dev/mem file, this file allowing to access to the physical addresses
	   of the system */
	fd = open("/dev/mem", O_RDWR);

	/* Mmap request a mapping of a physical address FPGA_BASE_ADDR
	   on the virtual address domain of the process.
	   Flags PROT_WRITE | PROT_READ allow the read and write on this memory area
	   Flag MAP_SHARED allowing to synchronize the memory without this flags the data */
	ptr_fpga = mmap(NULL, sizeof(int), PROT_WRITE | PROT_READ, MAP_SHARED, fd, FPGA_BASE_ADDR);
	if (ptr_fpga == MAP_FAILED) {
		perror("Cannot mmap\n");
	}

	/* Test read and write on the memory area */
	fprintf(stdout, "%d\n", *ptr_fpga);
	*ptr_fpga = 1;
	fprintf(stdout, "%d\n", *ptr_fpga);

	*ptr_fpga = 0xFFFFFFFF;

	/* Synchronize the cache with memory */
	msync(ptr_fpga, sizeof(int), MS_SYNC);

	/* Unmap the memory */
	munmap(ptr_fpga, sizeof(int));
}
