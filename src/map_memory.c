#include <string.h>

#include "map_memory.h"

/* Registers ADDR */

#define REG_CTRL                 0xA0180000 /* Control and status register, bit definition below */
#define REG_CTRL_BIT_START       1

#define bitset(uint32_t, nbit) 	((uint32_t) |= (1 <<(nbit)))
#define bitclear(uint32_t, nbit) 	((uint32_t) &= ~(1 <<(nbit)))
#define bitflip(uint32_t, nbit) 	((uint32_t) ^= (1 <<(nbit)))


#define start_0 			0
#define reset_0				1
#define start_1 			3
#define reset_1				4
#define start_2 			6
#define reset_2				7
#define start_3 			9
#define reset_3				10
#define start_4 			12
#define reset_4				13
#define start_5 			15
#define reset_5				16
#define start_6 			18
#define reset_6				19
#define start_7 			21
#define reset_7				22

union reg_ctrl {
	unsigned int reg;
	struct {
		unsigned int ctrl_start_0       : 1;
		unsigned int ctrl_reset_0       : 1;
		unsigned int st_end_process_0   : 1;
		unsigned int ctrl_start_1       : 1;
		unsigned int ctrl_reset_1       : 1;
		unsigned int st_end_process_1   : 1;
		unsigned int ctrl_start_2       : 1;
		unsigned int ctrl_reset_2       : 1;
		unsigned int st_end_process_2   : 1;
		unsigned int ctrl_start_3       : 1;
		unsigned int ctrl_reset_3       : 1;
		unsigned int st_end_process_3   : 1;
		unsigned int ctrl_start_4       : 1;
		unsigned int ctrl_reset_4       : 1;
		unsigned int st_end_process_4   : 1;
		unsigned int ctrl_start_5       : 1;
		unsigned int ctrl_reset_5       : 1;
		unsigned int st_end_process_5   : 1;
		unsigned int ctrl_start_6       : 1;
		unsigned int ctrl_reset_6       : 1;
		unsigned int st_end_process_6   : 1;
		unsigned int ctrl_start_7       : 1;
		unsigned int ctrl_reset_7       : 1;
		unsigned int st_end_process_7   : 1;
		unsigned int 	/*padding*/		: 8;
	} bits;
};

// 1st Accelerator - Known in block design as GENERATED_IP_1
#define REG_IN_0                 0x00A0000000
#define REG_OUT_0                0x00A0010000
#define REG_DURATION_LATCHED_0   0x00A0020000

// 2nd Accelerator - Known in block design as GENERATED_IP_2 
#define REG_IN_1                 0x00A0030000
#define REG_OUT_1                0x00A0040000
#define REG_DURATION_LATCHED_1   0x00A0050000

// 3rd Accelerator - Known in block design as GENERATED_IP_3 
#define REG_IN_2                 0x00A0060000
#define REG_OUT_2                0x00A0070000
#define REG_DURATION_LATCHED_2   0x00A0080000

// 4th Accelerator - Known in block design as GENERATED_IP_4 
#define REG_IN_3                 0x00A0090000
#define REG_OUT_3                0x00A00A0000
#define REG_DURATION_LATCHED_3   0x00A00B0000

// 5th Accelerator - Known in block design as GENERATED_IP_5 
#define REG_IN_4                 0x00A00C0000
#define REG_OUT_4                0x00A00D0000
#define REG_DURATION_LATCHED_4   0x00A00E0000

// 6th Accelerator - Known in block design as GENERATED_IP_6
#define REG_IN_5                 0x00A00F0000
#define REG_OUT_5                0x00A0100000
#define REG_DURATION_LATCHED_5   0x00A0110000

// 7th Accelerator - Known in block design as GENERATED_IP_7
#define REG_IN_6                 0x00A0120000
#define REG_OUT_6                0x00A0130000
#define REG_DURATION_LATCHED_6   0x00A0140000

// 8th Accelerator - Known in block design as GENERATED_IP_8
#define REG_IN_7                 0x00A0150000
#define REG_OUT_7                0x00A0160000
#define REG_DURATION_LATCHED_7   0x00A0170000

#define REG_MODE                 0x1

#define POLL_PERIOD_US           1
#define ACCEL_COUNT		  		 8

struct mmap_area {			//keeps track of the base addresses and sizes of the different registers
	off_t phy_addr;
	off_t param_offset;
	off_t result_offset;
};
struct mmap_area mmapped[ACCEL_COUNT] = { 0 }; 
off_t next_addr = 0;  // keeps track of what the next base address should be when mapping memory


/* File descriptor for /dev/mem open */
static int fd = -1;
/*pointers for the various registers*/
union  reg_ctrl *ptr_reg_ctrl_st     	= NULL;
static uint32_t *ptr_reg_in_0       	= NULL;
static uint32_t *ptr_reg_out_0       	= NULL;
static uint32_t *ptr_reg_dur_latched_0 	= NULL;
static uint32_t *ptr_reg_in_1       	= NULL;
static uint32_t *ptr_reg_out_1       	= NULL;
static uint32_t *ptr_reg_dur_latched_1 	= NULL;
static uint32_t *ptr_reg_in_2       	= NULL;
static uint32_t *ptr_reg_out_2       	= NULL;
static uint32_t *ptr_reg_dur_latched_2 	= NULL;
static uint32_t *ptr_reg_in_3       	= NULL;
static uint32_t *ptr_reg_out_3       	= NULL;
static uint32_t *ptr_reg_dur_latched_3 	= NULL;
static uint32_t *ptr_reg_in_4       	= NULL;
static uint32_t *ptr_reg_out_4       	= NULL;
static uint32_t *ptr_reg_dur_latched_4 	= NULL;
static uint32_t *ptr_reg_in_5       	= NULL;
static uint32_t *ptr_reg_out_5       	= NULL;
static uint32_t *ptr_reg_dur_latched_5 	= NULL;
static uint32_t *ptr_reg_in_6       	= NULL;
static uint32_t *ptr_reg_out_6       	= NULL;
static uint32_t *ptr_reg_dur_latched_6 	= NULL;
static uint32_t *ptr_reg_in_7       	= NULL;
static uint32_t *ptr_reg_out_7       	= NULL;
static uint32_t *ptr_reg_dur_latched_7 	= NULL;


#ifdef TIME_MEASURE
struct timespec ts_acc_start[ACCEL_COUNT] = { 0 };
struct timespec ts_acc_end[ACCEL_COUNT] = { 0 };
#endif /* TIME_MEASURE */

static int open_devmem()
{
	/* /dev/mem is an interface allowing to access the physical memory addr */
	fd = open("/dev/mem", O_RDWR | O_SYNC);
	if (fd == -1) {
		perror("Cannot open /dev/mem file");
		return -1;
	}

	return 0;
}

int map_phys_addr(off_t phy_addr, size_t len, void **virt_ptr)
{
	/* If /dev/mem is not opened */
	if (fd == -1) {
		if ( open_devmem() ) {
			return -1;
		}
	}

	DEBUG_PRINT("%s - 0x%x with size %zu bytes\n",
	            __func__, phy_addr, len);

	/* Map physical address in virtual mem, virtual mem addr is stored in virt_ptr */
	*virt_ptr = mmap(NULL,
	                 len,
	                 PROT_WRITE | PROT_READ,
	                 MAP_SHARED,
	                 fd,
	                 phy_addr);
	if (*virt_ptr == MAP_FAILED) {
		perror("Cannot mmap");
		return -1;
	}

	return 0;
}

//stores the physical addresses for each IP.
//the phy_addr must be a multiple of the page size, here it is 4096
void init_accelerator_memory(off_t param_offset,
                             off_t result_offset,
							 off_t result_len,
							 int accel)
{	
	if (next_addr != 0 ){
		mmapped[accel].phy_addr = next_addr;
	}
	else{
		mmapped[accel].phy_addr = MEM_DMA_BASE;	
	}

	if (mmapped[accel].phy_addr % 4096){
		mmapped[accel].phy_addr += (4096 - (mmapped[accel].phy_addr % 4096));
	}
	mmapped[accel].param_offset = param_offset;
	mmapped[accel].result_offset = result_offset;
	next_addr = mmapped[accel].phy_addr + mmapped[accel].param_offset + mmapped[accel].result_offset + result_len;
	DEBUG_PRINT("%s - accel %d : Addr=0x%x, param off=%ld, result off=%ld, result len=%ld\n",
	        __func__, (accel+1), mmapped[accel].phy_addr, mmapped[accel].param_offset, mmapped[accel].result_offset, result_len);		
	
}
//unmaps the virtual from the physical memory
int unmap_phys_addr(void *virt_ptr, size_t len)
{
	if ( munmap(virt_ptr, sizeof(int)) ) {
		perror("Cannot unmap");
		return -1;
	}

	return 0;
}

//initialises the various registers for the data in, out and the duration for the processing time.
int init_dma()
{
		if (fd != -1) {
				fprintf(stdout, "%s - Already initialized \n", __func__);
				return -1;
			}
		if ( open_devmem() ) {
			return -1;
		}
			
		//control register
		if ( map_phys_addr(REG_CTRL, sizeof(uint32_t), (void **)&ptr_reg_ctrl_st) ) {
			fprintf(stdout, "Mmap for control register failed\n");
			return -1;
		}
		//sets "parameter" bits of the control register, "1" is input, "0" is output
		(ptr_reg_ctrl_st + REG_MODE)->reg = 0x24924924;
		//(ptr_reg_ctrl_st)->reg = 0x00000000;
		DEBUG_PRINT("GPIO_0 setting bits: 0x%x\n", ((ptr_reg_ctrl_st + REG_MODE)->reg));
		DEBUG_PRINT("GPIO_0 data: 0x%x\n", ptr_reg_ctrl_st->reg);		
		//resets all IPs via control register bit field
		ptr_reg_ctrl_st->bits.ctrl_reset_0 = 1;
		ptr_reg_ctrl_st->bits.ctrl_reset_1 = 1;
		ptr_reg_ctrl_st->bits.ctrl_reset_2 = 1;
		ptr_reg_ctrl_st->bits.ctrl_reset_3 = 1;
		ptr_reg_ctrl_st->bits.ctrl_reset_4 = 1;
		ptr_reg_ctrl_st->bits.ctrl_reset_5 = 1;
		ptr_reg_ctrl_st->bits.ctrl_reset_6 = 1;
		ptr_reg_ctrl_st->bits.ctrl_reset_7 = 1;
		DEBUG_PRINT("GPIO_0 data: 0x%x\n", ptr_reg_ctrl_st->reg);
		ptr_reg_ctrl_st->bits.ctrl_reset_0 = 0;
		ptr_reg_ctrl_st->bits.ctrl_reset_1 = 0;
		ptr_reg_ctrl_st->bits.ctrl_reset_2 = 0;
		ptr_reg_ctrl_st->bits.ctrl_reset_3 = 0;
		ptr_reg_ctrl_st->bits.ctrl_reset_4 = 0;
		ptr_reg_ctrl_st->bits.ctrl_reset_5 = 0;
		ptr_reg_ctrl_st->bits.ctrl_reset_6 = 0;
		ptr_reg_ctrl_st->bits.ctrl_reset_7 = 0;
	//	/* Configure register input / output mode
		//IP1 registers
			if ( map_phys_addr(REG_IN_0, sizeof(uint32_t), (void **)&ptr_reg_in_0) ) {
				fprintf(stdout, "Mmap for input register 0 failed\n");
				return -1;
			}
			if ( map_phys_addr(REG_OUT_0, sizeof(uint32_t), (void **)&ptr_reg_out_0) ) {
				fprintf(stdout, "Mmap for output register 0 failed\n");
				return -1;
			}
			if ( map_phys_addr(REG_DURATION_LATCHED_0, sizeof(uint32_t), (void **)&ptr_reg_dur_latched_0) ) {
				fprintf(stdout, "Mmap for latched duration register 0 failed\n");
				return -1;
			}
			*(ptr_reg_dur_latched_0 + REG_MODE) = 0xFFFFFFFF;
			DEBUG_PRINT("IP_1 Mapping and register are initialized\n");	
		//IP2 registers
		//	DEBUG_PRINT("accel %d ", accel);
			if ( map_phys_addr(REG_IN_1, sizeof(uint32_t), (void **)&ptr_reg_in_1) ) {
				fprintf(stdout, "Mmap for input register 1 failed\n");
				return -1;
			}
			if ( map_phys_addr(REG_OUT_1, sizeof(uint32_t), (void **)&ptr_reg_out_1) ) {
				fprintf(stdout, "Mmap for output register 1 failed\n");
				return -1;
			}
			if ( map_phys_addr(REG_DURATION_LATCHED_1, sizeof(uint32_t), (void **)&ptr_reg_dur_latched_1) ) {
				fprintf(stdout, "Mmap for latched duration register 1 failed\n");
				return -1;
			}
			*(ptr_reg_dur_latched_1 + REG_MODE) = 0xFFFFFFFF;
			DEBUG_PRINT("IP_2 Mapping and register are initialized\n");
		//IP3 registers
		//	DEBUG_PRINT("accel %d ", accel);
			if ( map_phys_addr(REG_IN_2, sizeof(uint32_t), (void **)&ptr_reg_in_2) ) {
				fprintf(stdout, "Mmap for input register 2 failed\n");
				return -1;
			}
			if ( map_phys_addr(REG_OUT_2, sizeof(uint32_t), (void **)&ptr_reg_out_2) ) {
				fprintf(stdout, "Mmap for output register 2 failed\n");
				return -1;
			}
			if ( map_phys_addr(REG_DURATION_LATCHED_2, sizeof(uint32_t), (void **)&ptr_reg_dur_latched_2) ) {
				fprintf(stdout, "Mmap for latched duration register 2 failed\n");
				return -1;
			}
			*(ptr_reg_dur_latched_2 + REG_MODE) = 0xFFFFFFFF;
			DEBUG_PRINT("IP_3 Mapping and register are initialized\n");	
		//IP4 registers
		//	DEBUG_PRINT("accel %d ", accel);
			if ( map_phys_addr(REG_IN_3, sizeof(uint32_t), (void **)&ptr_reg_in_3) ) {
				fprintf(stdout, "Mmap for input register 3 failed\n");
				return -1;
			}
			if ( map_phys_addr(REG_OUT_3, sizeof(uint32_t), (void **)&ptr_reg_out_3) ) {
				fprintf(stdout, "Mmap for output register 3 failed\n");
				return -1;
			}
			if ( map_phys_addr(REG_DURATION_LATCHED_3, sizeof(uint32_t), (void **)&ptr_reg_dur_latched_3) ) {
				fprintf(stdout, "Mmap for latched duration register 3 failed\n");
				return -1;
			}
			*(ptr_reg_dur_latched_3 + REG_MODE) = 0xFFFFFFFF;
			DEBUG_PRINT("IP_4 Mapping and register are initialized\n");	
		//IP5 registers
		//	DEBUG_PRINT("accel %d ", accel);
			if ( map_phys_addr(REG_IN_4, sizeof(uint32_t), (void **)&ptr_reg_in_4) ) {
				fprintf(stdout, "Mmap for input register 4 failed\n");
				return -1;
			}
			if ( map_phys_addr(REG_OUT_4, sizeof(uint32_t), (void **)&ptr_reg_out_4) ) {
				fprintf(stdout, "Mmap for output register 4 failed\n");
				return -1;
			}
			if ( map_phys_addr(REG_DURATION_LATCHED_4, sizeof(uint32_t), (void **)&ptr_reg_dur_latched_4) ) {
				fprintf(stdout, "Mmap for latched duration register 4 failed\n");
				return -1;
			}
			*(ptr_reg_dur_latched_4 + REG_MODE) = 0xFFFFFFFF;
			DEBUG_PRINT("IP_5 Mapping and register are initialized\n");	
		//IP6 registers
		//	DEBUG_PRINT("accel %d ", accel);
			if ( map_phys_addr(REG_IN_5, sizeof(uint32_t), (void **)&ptr_reg_in_5) ) {
				fprintf(stdout, "Mmap for input register 5 failed\n");
				return -1;
			}
			if ( map_phys_addr(REG_OUT_5, sizeof(uint32_t), (void **)&ptr_reg_out_5) ) {
				fprintf(stdout, "Mmap for output register 5 failed\n");
				return -1;
			}
			if ( map_phys_addr(REG_DURATION_LATCHED_5, sizeof(uint32_t), (void **)&ptr_reg_dur_latched_5) ) {
				fprintf(stdout, "Mmap for latched duration register 5 failed\n");
				return -1;
			}
			*(ptr_reg_dur_latched_5 + REG_MODE) = 0xFFFFFFFF;
			DEBUG_PRINT("IP_6 Mapping and register are initialized\n");	
		//IP7 registers
		//	DEBUG_PRINT("accel %d ", accel);
			if ( map_phys_addr(REG_IN_6, sizeof(uint32_t), (void **)&ptr_reg_in_6) ) {
				fprintf(stdout, "Mmap for input register 6 failed\n");
				return -1;
			}
			if ( map_phys_addr(REG_OUT_6, sizeof(uint32_t), (void **)&ptr_reg_out_6) ) {
				fprintf(stdout, "Mmap for output register 6 failed\n");
				return -1;
			}
			if ( map_phys_addr(REG_DURATION_LATCHED_6, sizeof(uint32_t), (void **)&ptr_reg_dur_latched_6) ) {
				fprintf(stdout, "Mmap for latched duration register 6 failed\n");
				return -1;
			}
			*(ptr_reg_dur_latched_6 + REG_MODE) = 0xFFFFFFFF;
			DEBUG_PRINT("IP_7 Mapping and register are initialized\n");	
		//IP8 registers
		//	DEBUG_PRINT("accel %d ", accel);
			if ( map_phys_addr(REG_IN_7, sizeof(uint32_t), (void **)&ptr_reg_in_7) ) {
				fprintf(stdout, "Mmap for input register 7 failed\n");
				return -1;
			}
			if ( map_phys_addr(REG_OUT_7, sizeof(uint32_t), (void **)&ptr_reg_out_7) ) {
				fprintf(stdout, "Mmap for output register 7 failed\n");
				return -1;
			}
			if ( map_phys_addr(REG_DURATION_LATCHED_7, sizeof(uint32_t), (void **)&ptr_reg_dur_latched_7) ) {
				fprintf(stdout, "Mmap for latched duration register 7 failed\n");
				return -1;
			}
			*(ptr_reg_dur_latched_7 + REG_MODE) = 0xFFFFFFFF;
			DEBUG_PRINT("IP_8 Mapping and register are initialized\n");				
		DEBUG_PRINT(" END DMA init GPIO_0 setting bits: 0x%x\n", ((ptr_reg_ctrl_st + REG_MODE)->reg));
	return 0;

	DEBUG_PRINT(" END DMA init GPIO_0 setting bits: 0x%x\n", ((ptr_reg_ctrl_st + REG_MODE)->reg));
}


#define MEMCPY_LEN 8
/*creates a connection between the CPU RAM half and the FPGA RAM half, then copies the data from the one to the other
for the specified accelerator*/ 
int cp_param_to_fpga(struct fpga_param *param, int accel)
{
#ifdef TIME_MEASURE
	struct timespec t1;
	struct timespec t2;
	if (clock_gettime(CLOCK_MONOTONIC, &t1)) {
		perror("Cannot get time\n");
		return -1;
	}
#endif /* TIME_MEASURE */
	//fprintf(stdout, "CP_param_to_FPGA Start mmap %d\n", accel);
	DEBUG_PRINT("%s - accel %d param size: %ld, fd: %d phy addr: 0x%x, param offset: %ld \n",
		__func__, accel+1, param->len, fd, mmapped[accel].phy_addr, mmapped[accel].param_offset);
	void *mmap_ptr = mmap(NULL,
	                      param->len,
	                      PROT_WRITE | PROT_READ,
	                      MAP_SHARED,
	                      fd,
	                      (mmapped[accel].phy_addr + mmapped[accel].param_offset));
	if (mmap_ptr == MAP_FAILED) {
		perror("Cannot mmap");
		return -1;
	}

	/* A Bus error is encountered when we do a memcpy > to 8 bytes
	   So the copy is limited to maximum 8 bytes */
	int i;
	size_t cp_len = MEMCPY_LEN;
	for (i = 0; i<param->len; i += MEMCPY_LEN) {
		if (i + MEMCPY_LEN > param->len) {
			cp_len = param->len - i;
		} else {
			cp_len = MEMCPY_LEN;
		}
		DEBUG_PRINT("%s - i=%d  cp_len=%zu\n", __func__, i, cp_len);
		memcpy(mmap_ptr + i, param->p + i, cp_len);
	}
	munmap(mmap_ptr, param->len);

#ifdef TIME_MEASURE
	if (clock_gettime(CLOCK_MONOTONIC, &t2)) {
		perror("Cannot get time\n");
		return -1;
	}
	struct timespec t_memcpy = diff_ts(&t1, &t2);
	fprintf(stdout,
	        "Accelerator %d: Copy parameters from CPU to FPGA: %zu "
	        "bytes in %zu s and %zu ns\n", accel+1, 
	        param->len, t_memcpy.tv_sec, t_memcpy.tv_nsec);
#endif /* TIME_MEASURE */
	DEBUG_PRINT("%s gpio_0 values on exit 0x%x\n",__func__, (ptr_reg_ctrl_st->reg));
	return 0;
}
/*maps a link between the result register and memory, then copies the data to memory*/
int cp_result_from_fpga(struct fpga_param *result, int accel)
{
#ifdef TIME_MEASURE
	struct timespec t1;
	struct timespec t2;
	if (clock_gettime(CLOCK_MONOTONIC, &t1)) {
		perror("Cannot get time\n");
		return -1;
	}
#endif /* TIME_MEASURE */
	DEBUG_PRINT("%s\n", __func__);
	void *mmap_ptr = mmap(NULL,
	                      mmapped[accel].param_offset + result->len,
	                      PROT_WRITE | PROT_READ,
	                      MAP_SHARED,
	                      fd,
	                      mmapped[accel].phy_addr);
	if (mmap_ptr == MAP_FAILED) {
		perror("Cannot mmap");
		return -1;
	}

	/* A Bus error is encountered when we do a memcpy > to 8 bytes
	   So the copy is limited to maximum 8 bytes */
	size_t cp_len = MEMCPY_LEN;
	int i=0;
	
	/* If result offset is not aligned on MEMCPY_LEN bytes 
	   first copy shall be inferior to MEMCPY_LEN */
	DEBUG_PRINT("%s - result_offset \% MEMCPY_LEN: %d\n",
		__func__, mmapped[accel].result_offset % MEMCPY_LEN);
	if (mmapped[accel].result_offset % MEMCPY_LEN) {
		cp_len = MEMCPY_LEN - mmapped[accel].result_offset % MEMCPY_LEN;
		i = cp_len;
		memcpy(result->p, mmap_ptr + mmapped[accel].result_offset, cp_len);
	/*	DEBUG_PRINT("%s - reading addr: 0x%x, value: %d\n",
		__func__, mmap_ptr + mmapped[accel].result_offset, result->p);*/
	}

	for (i; i<result->len; i += MEMCPY_LEN) {
		if (i + MEMCPY_LEN > result->len) {
			cp_len = result->len - i;
		} else {
			cp_len = MEMCPY_LEN;
		}
		/* TODO debug */
		DEBUG_PRINT("%s - i=%d  cp_len=%zu\n", __func__, i, cp_len);
		memcpy(result->p + i, mmap_ptr + mmapped[accel].result_offset + i, cp_len);
		DEBUG_PRINT("%s - reading addr: 0x%x\n",
		__func__, mmapped[accel].phy_addr + mmapped[accel].result_offset);
	}

	munmap(mmap_ptr, result->len);

#ifdef TIME_MEASURE
	if (clock_gettime(CLOCK_MONOTONIC, &t2)) {
		perror("Cannot get time\n");
		return -1;
	}
	struct timespec t_memcpy = diff_ts(&t1, &t2);
	fprintf(stdout,
	        "Accelerator %d: Copy result from FPGA to CPU: %zu "
	        "bytes in %zu s and %zu ns\n", accel+1, 
	        result->len, t_memcpy.tv_sec, t_memcpy.tv_nsec);
#endif /* TIME_MEASURE */
DEBUG_PRINT("%s gpio_0 values on exit 0x%x\n",__func__, (ptr_reg_ctrl_st->reg));
	return 0;

}

int start_accelerator(int accel)
{
	if (ptr_reg_ctrl_st == NULL) {
		fprintf(stdout, "%s not initialized\n", __func__);
		return -1;
	}
	DEBUG_PRINT("Starting processing of accelerator: %d\n",accel);
	DEBUG_PRINT("%s gpio_0 values on entrance 0x%x\n",__func__, (ptr_reg_ctrl_st->reg));
	DEBUG_PRINT("reg_in_%d pointer at 0x%d\n", accel+1, DMA_ADDR(mmapped[accel].phy_addr));
	DEBUG_PRINT("reg_out_%d pointer at 0x%d\n", accel+1, DMA_ADDR(mmapped[accel].phy_addr + mmapped[accel].result_offset));
	switch (accel){
		case 0:
			*ptr_reg_in_0 = DMA_ADDR(mmapped[accel].phy_addr); //replaces set_in_x
			*ptr_reg_out_0 = DMA_ADDR(mmapped[accel].phy_addr + mmapped[accel].result_offset); // replaces set_out_x
			bitset(ptr_reg_ctrl_st->reg, start_0);
			DEBUG_PRINT("%s gpio_0 values after start high 0x%x\n",__func__, (ptr_reg_ctrl_st->reg));
			fprintf(stdout, "\n"); // a necessary "delay" otherwise the IP doesn't register the change 
			bitclear(ptr_reg_ctrl_st->reg, start_0);
			DEBUG_PRINT("%s gpio_0 values after start low 0x%x\n",__func__, (ptr_reg_ctrl_st->reg));
		break;
		case 1:
			*ptr_reg_in_1 = DMA_ADDR(mmapped[accel].phy_addr); //replaces set_in_x
			*ptr_reg_out_1 = DMA_ADDR(mmapped[accel].phy_addr + mmapped[accel].result_offset); // replaces set_out_x
			//ptr_reg_ctrl_st->bits.ctrl_start_1 = 1;
			bitset(ptr_reg_ctrl_st->reg, start_1);
			DEBUG_PRINT("%s gpio_0 values after start high 0x%x\n",__func__, (ptr_reg_ctrl_st->reg));
			fprintf(stdout, "\n");
			//ptr_reg_ctrl_st->bits.ctrl_start_1 = 0;
			bitclear(ptr_reg_ctrl_st->reg, start_1);
			DEBUG_PRINT("%s gpio_0 values after start low 0x%x\n",__func__, (ptr_reg_ctrl_st->reg));
		break;
		case 2:
			*ptr_reg_in_2 = DMA_ADDR(mmapped[accel].phy_addr); //replaces set_in_x
			*ptr_reg_out_2 = DMA_ADDR(mmapped[accel].phy_addr + mmapped[accel].result_offset); // replaces set_out_x
			//ptr_reg_ctrl_st->bits.ctrl_start_2 = 1;
			bitset(ptr_reg_ctrl_st->reg, start_2);
			DEBUG_PRINT("%s gpio_0 values after start high 0x%x\n",__func__, (ptr_reg_ctrl_st->reg));
			fprintf(stdout, "\n");
			//ptr_reg_ctrl_st->bits.ctrl_start_2 = 0;
			bitclear(ptr_reg_ctrl_st->reg, start_2);
			DEBUG_PRINT("%s gpio_0 values after start low 0x%x\n",__func__, (ptr_reg_ctrl_st->reg));
		break;
		case 3:
			*ptr_reg_in_3 = DMA_ADDR(mmapped[accel].phy_addr); //replaces set_in_x
			*ptr_reg_out_3 = DMA_ADDR(mmapped[accel].phy_addr + mmapped[accel].result_offset); // replaces set_out_x
			//ptr_reg_ctrl_st->bits.ctrl_start_3 = 1;
			bitset(ptr_reg_ctrl_st->reg, start_3);
			DEBUG_PRINT("%s gpio_0 values after start high 0x%x\n",__func__, (ptr_reg_ctrl_st->reg));
			fprintf(stdout, "\n");
			//ptr_reg_ctrl_st->bits.ctrl_start_3 = 0;
			bitclear(ptr_reg_ctrl_st->reg, start_3);
			DEBUG_PRINT("%s gpio_0 values after start low 0x%x\n",__func__, (ptr_reg_ctrl_st->reg));
		break;
		case 4:
			*ptr_reg_in_4 = DMA_ADDR(mmapped[accel].phy_addr); //replaces set_in_x
			*ptr_reg_out_4 = DMA_ADDR(mmapped[accel].phy_addr + mmapped[accel].result_offset); // replaces set_out_x
			//ptr_reg_ctrl_st->bits.ctrl_start_4 = 1;
			bitset(ptr_reg_ctrl_st->reg, start_4);
			DEBUG_PRINT("%s gpio_0 values after start high 0x%x\n",__func__, (ptr_reg_ctrl_st->reg));
			fprintf(stdout, "\n");
			//ptr_reg_ctrl_st->bits.ctrl_start_4 = 0;
			bitclear(ptr_reg_ctrl_st->reg, start_4);
			DEBUG_PRINT("%s gpio_0 values after start low 0x%x\n",__func__, (ptr_reg_ctrl_st->reg));
		break;
		case 5:
			*ptr_reg_in_5 = DMA_ADDR(mmapped[accel].phy_addr); //replaces set_in_x
			*ptr_reg_out_5 = DMA_ADDR(mmapped[accel].phy_addr + mmapped[accel].result_offset); // replaces set_out_x
			//ptr_reg_ctrl_st->bits.ctrl_start_5 = 1;
			bitset(ptr_reg_ctrl_st->reg, start_5);
			DEBUG_PRINT("%s gpio_0 values after start high 0x%x\n",__func__, (ptr_reg_ctrl_st->reg));
			fprintf(stdout, "\n");
			//ptr_reg_ctrl_st->bits.ctrl_start_5 = 0;
			bitclear(ptr_reg_ctrl_st->reg, start_5);
			DEBUG_PRINT("%s gpio_0 values after start low 0x%x\n",__func__, (ptr_reg_ctrl_st->reg));
		break;
		case 6:
			*ptr_reg_in_6 = DMA_ADDR(mmapped[accel].phy_addr); //replaces set_in_x
			*ptr_reg_out_6 = DMA_ADDR(mmapped[accel].phy_addr + mmapped[accel].result_offset); // replaces set_out_x
			//ptr_reg_ctrl_st->bits.ctrl_start_6 = 1;
			bitset(ptr_reg_ctrl_st->reg, start_6);
			DEBUG_PRINT("%s gpio_0 values after start high 0x%x\n",__func__, (ptr_reg_ctrl_st->reg));
			fprintf(stdout, "\n");
			//ptr_reg_ctrl_st->bits.ctrl_start_6 = 0;
			bitclear(ptr_reg_ctrl_st->reg, start_6);
			DEBUG_PRINT("%s gpio_0 values after start low 0x%x\n",__func__, (ptr_reg_ctrl_st->reg));
		break;
		case 7:
			*ptr_reg_in_7 = DMA_ADDR(mmapped[accel].phy_addr); //replaces set_in_x
			*ptr_reg_out_7 = DMA_ADDR(mmapped[accel].phy_addr + mmapped[accel].result_offset); // replaces set_out_x
			//ptr_reg_ctrl_st->bits.ctrl_start_7 = 1;
			bitset(ptr_reg_ctrl_st->reg, start_7);
			DEBUG_PRINT("%s gpio_0 values after start high 0x%x\n",__func__, (ptr_reg_ctrl_st->reg));
			fprintf(stdout, "\n");
			//ptr_reg_ctrl_st->bits.ctrl_start_7 = 0;
			bitclear(ptr_reg_ctrl_st->reg, start_7);
			DEBUG_PRINT("%s gpio_0 values after start low 0x%x\n",__func__, (ptr_reg_ctrl_st->reg));
		break;
		default:
			fprintf(stdout, "Incorrect accelerator selection\n");
		break;
	}
	DEBUG_PRINT("%s gpio_0 values on exit 0x%x\n",__func__, (ptr_reg_ctrl_st->reg));
#ifdef TIME_MEASURE
	if (clock_gettime(CLOCK_MONOTONIC, &ts_acc_start[accel])) {
		perror("Cannot get time\n");
		return -1;
	}
#endif /* TIME_MEASURE */
	DEBUG_PRINT("Accel %d successfully started\n",accel+1);
	return 0;
}
//polls the done signal to determine if an IP is finished
int wait_accelerator(struct fpga_param *result, int accel)
{
	if (ptr_reg_ctrl_st == NULL) {
		fprintf(stdout, "%s not initialized\n", __func__);
		return -1;
	}

	DEBUG_PRINT("%s - Start polling ", __func__);
	DEBUG_PRINT("%d \n", accel+1);	
	DEBUG_PRINT("gpio_0 values on entrance 0x%x\n", (ptr_reg_ctrl_st->reg));
	switch (accel){
	case 0:
			while (ptr_reg_ctrl_st->bits.st_end_process_0 != 1) {
				DEBUG_PRINT("waiting for done bit %d\n", (ptr_reg_ctrl_st->bits.st_end_process_0));
				usleep(POLL_PERIOD_US);
			}
		break;
		case 1:
			while (ptr_reg_ctrl_st->bits.st_end_process_1 != 1) {
				DEBUG_PRINT("waiting for done bit %d\n", (ptr_reg_ctrl_st->bits.st_end_process_1));
				usleep(POLL_PERIOD_US);
			}
		break;
		case 2:
			while (ptr_reg_ctrl_st->bits.st_end_process_2 != 1) {
				DEBUG_PRINT("waiting for done bit %d\n", (ptr_reg_ctrl_st->bits.st_end_process_2));
				usleep(POLL_PERIOD_US);
			}
		break;
		case 3:
			while (ptr_reg_ctrl_st->bits.st_end_process_3 != 1) {
				DEBUG_PRINT("waiting for done bit %d\n", (ptr_reg_ctrl_st->bits.st_end_process_3));
				usleep(POLL_PERIOD_US);
			}
		break;
		case 4:
			while (ptr_reg_ctrl_st->bits.st_end_process_4 != 1) {
				DEBUG_PRINT("waiting for done bit %d\n", (ptr_reg_ctrl_st->bits.st_end_process_4));
			usleep(POLL_PERIOD_US);
			}
		break;
		case 5:
			while (ptr_reg_ctrl_st->bits.st_end_process_5 != 1) {
				DEBUG_PRINT("waiting for done bit %d\n", (ptr_reg_ctrl_st->bits.st_end_process_5));
			usleep(POLL_PERIOD_US);
			}
		break;
		case 6:
			while (ptr_reg_ctrl_st->bits.st_end_process_6 != 1) {
				DEBUG_PRINT("waiting for done bit %d\n", (ptr_reg_ctrl_st->bits.st_end_process_6));
			usleep(POLL_PERIOD_US);
			}
		break;
		case 7:
			while (ptr_reg_ctrl_st->bits.st_end_process_7 != 1) {
				DEBUG_PRINT("waiting for done bit %d\n", (ptr_reg_ctrl_st->bits.st_end_process_7));
			usleep(POLL_PERIOD_US);
			}
		break;
				default:
			fprintf(stdout, "Non-existant accelerator selected.\n");
		break;
	}
	DEBUG_PRINT(" End polling\n", __func__);
	DEBUG_PRINT("%s gpio_0 values on exit 0x%x\n", __func__, (ptr_reg_ctrl_st->reg));
#ifdef TIME_MEASURE
	if (clock_gettime(CLOCK_MONOTONIC, &ts_acc_end[accel])) {
		perror("Cannot get time\n");
		return -1;
	}

	struct timespec t_processing = diff_ts(&ts_acc_start[accel], &ts_acc_end[accel]);
fprintf(stdout, "Accelerator %d: Processing time: %zu s and %zu ns\n", accel+1, 
	        t_processing.tv_sec, t_processing.tv_nsec);
	fprintf(stdout, "Accelerator %d:  Processing time from FPGA: %u ns\n", accel+1, get_time_ns_FPGA(accel));
#endif /* TIME_MEASURE */
	
	cp_result_from_fpga(result, accel);

	return 0;
}
/*reads the value stored in the duration register for a specific accelerator*/
uint32_t get_reg_dur(int accel)
{
	switch (accel){
		case 0:
			DEBUG_PRINT("%s - Read FPGA processing time = %u cycles\n", __func__, 
	        *ptr_reg_dur_latched_0);
			return *ptr_reg_dur_latched_0;
		case 1:
			DEBUG_PRINT("%s - Read FPGA processing time = %u cycles\n", __func__, 
	        *ptr_reg_dur_latched_1);
			return *ptr_reg_dur_latched_1;
		case 2:
			DEBUG_PRINT("%s - Read FPGA processing time = %u cycles\n", __func__, 
	        *ptr_reg_dur_latched_2);
			return *ptr_reg_dur_latched_2;
		case 3:
			DEBUG_PRINT("%s - Read FPGA processing time = %u cycles\n", __func__, 
	        *ptr_reg_dur_latched_3);
			return *ptr_reg_dur_latched_3;
		case 4:
			DEBUG_PRINT("%s - Read FPGA processing time = %u cycles\n", __func__, 
	        *ptr_reg_dur_latched_4);
			return *ptr_reg_dur_latched_4;
		case 5:
			DEBUG_PRINT("%s - Read FPGA processing time = %u cycles\n", __func__, 
	        *ptr_reg_dur_latched_5);
			return *ptr_reg_dur_latched_5;
		case 6:
			DEBUG_PRINT("%s - Read FPGA processing time = %u cycles\n", __func__, 
	        *ptr_reg_dur_latched_6);
			return *ptr_reg_dur_latched_6;
		case 7:
			DEBUG_PRINT("%s - Read FPGA processing time = %u cycles\n", __func__, 
	        *ptr_reg_dur_latched_7);
			return *ptr_reg_dur_latched_7;
		default:
			("Accelerator selection error\n");
			break;	
}
}


void print_ctrl_st_reg(void)
{
	union reg_ctrl reg_ctrl_val = { 0 };
	reg_ctrl_val.reg = ptr_reg_ctrl_st->reg;

	fprintf(stdout, "Ctrl / Status register value : 0x%X\n", reg_ctrl_val.reg);
	fprintf(stdout, "Ctrl_Start_1 = %u\n", reg_ctrl_val.bits.ctrl_start_0);
	fprintf(stdout, "Ctrl_Reset_1 = %u\n", reg_ctrl_val.bits.ctrl_reset_0);
	fprintf(stdout, "Ctrl_Start_2 = %u\n", reg_ctrl_val.bits.ctrl_start_1);
	fprintf(stdout, "Ctrl_Reset_2 = %u\n", reg_ctrl_val.bits.ctrl_reset_1);
	fprintf(stdout, "Ctrl_Start_3 = %u\n", reg_ctrl_val.bits.ctrl_start_2);
	fprintf(stdout, "Ctrl_Reset_3 = %u\n", reg_ctrl_val.bits.ctrl_reset_2);
	fprintf(stdout, "Ctrl_Start_4 = %u\n", reg_ctrl_val.bits.ctrl_start_3);
	fprintf(stdout, "Ctrl_Reset_4 = %u\n", reg_ctrl_val.bits.ctrl_reset_3);
	fprintf(stdout, "Ctrl_Start_5 = %u\n", reg_ctrl_val.bits.ctrl_start_4);
	fprintf(stdout, "Ctrl_Reset_5 = %u\n", reg_ctrl_val.bits.ctrl_reset_4);
	fprintf(stdout, "Ctrl_Start_6 = %u\n", reg_ctrl_val.bits.ctrl_start_5);
	fprintf(stdout, "Ctrl_Reset_6 = %u\n", reg_ctrl_val.bits.ctrl_reset_5);
	fprintf(stdout, "Ctrl_Start_7 = %u\n", reg_ctrl_val.bits.ctrl_start_6);
	fprintf(stdout, "Ctrl_Reset_7 = %u\n", reg_ctrl_val.bits.ctrl_reset_6);
	fprintf(stdout, "Ctrl_Start_8 = %u\n", reg_ctrl_val.bits.ctrl_start_7);
	fprintf(stdout, "Ctrl_Reset_8 = %u\n", reg_ctrl_val.bits.ctrl_reset_7);
	fprintf(stdout, "Status_end_process_1 = %u\n", reg_ctrl_val.bits.st_end_process_0);
	fprintf(stdout, "Status_end_process_2 = %u\n", reg_ctrl_val.bits.st_end_process_1);
	fprintf(stdout, "Status_end_process_3 = %u\n", reg_ctrl_val.bits.st_end_process_2);
	fprintf(stdout, "Status_end_process_4 = %u\n", reg_ctrl_val.bits.st_end_process_3);
	fprintf(stdout, "Status_end_process_5 = %u\n", reg_ctrl_val.bits.st_end_process_4);
	fprintf(stdout, "Status_end_process_6 = %u\n", reg_ctrl_val.bits.st_end_process_5);
	fprintf(stdout, "Status_end_process_7 = %u\n", reg_ctrl_val.bits.st_end_process_6);
	fprintf(stdout, "Status_end_process_8 = %u\n", reg_ctrl_val.bits.st_end_process_7);
}



void print_ctrl_st_reg_tri(void)
{
	union reg_ctrl reg_ctrl_val = { 0 };
	reg_ctrl_val.reg = (ptr_reg_ctrl_st + REG_MODE)->reg;

	fprintf(stdout, "Ctrl / Status register IO value : 0x%X\n", reg_ctrl_val.reg);
	fprintf(stdout, "Ctrl_Start_1 = %u\n", reg_ctrl_val.bits.ctrl_start_0);
	fprintf(stdout, "Ctrl_Reset_1 = %u\n", reg_ctrl_val.bits.ctrl_reset_0);
	fprintf(stdout, "Ctrl_Start_2 = %u\n", reg_ctrl_val.bits.ctrl_start_1);
	fprintf(stdout, "Ctrl_Reset_2 = %u\n", reg_ctrl_val.bits.ctrl_reset_1);
	fprintf(stdout, "Ctrl_Start_3 = %u\n", reg_ctrl_val.bits.ctrl_start_2);
	fprintf(stdout, "Ctrl_Reset_3 = %u\n", reg_ctrl_val.bits.ctrl_reset_2);
	fprintf(stdout, "Ctrl_Start_4 = %u\n", reg_ctrl_val.bits.ctrl_start_3);
	fprintf(stdout, "Ctrl_Reset_4 = %u\n", reg_ctrl_val.bits.ctrl_reset_3);
	fprintf(stdout, "Ctrl_Start_5 = %u\n", reg_ctrl_val.bits.ctrl_start_4);
	fprintf(stdout, "Ctrl_Reset_5 = %u\n", reg_ctrl_val.bits.ctrl_reset_4);
	fprintf(stdout, "Ctrl_Start_6 = %u\n", reg_ctrl_val.bits.ctrl_start_5);
	fprintf(stdout, "Ctrl_Reset_6 = %u\n", reg_ctrl_val.bits.ctrl_reset_5);
	fprintf(stdout, "Ctrl_Start_7 = %u\n", reg_ctrl_val.bits.ctrl_start_6);
	fprintf(stdout, "Ctrl_Reset_7 = %u\n", reg_ctrl_val.bits.ctrl_reset_6);
	fprintf(stdout, "Ctrl_Start_8 = %u\n", reg_ctrl_val.bits.ctrl_start_7);
	fprintf(stdout, "Ctrl_Reset_8 = %u\n", reg_ctrl_val.bits.ctrl_reset_7);
	fprintf(stdout, "Status_end_process_1 = %u\n", reg_ctrl_val.bits.st_end_process_0);
	fprintf(stdout, "Status_end_process_2 = %u\n", reg_ctrl_val.bits.st_end_process_1);
	fprintf(stdout, "Status_end_process_3 = %u\n", reg_ctrl_val.bits.st_end_process_2);
	fprintf(stdout, "Status_end_process_4 = %u\n", reg_ctrl_val.bits.st_end_process_3);
	fprintf(stdout, "Status_end_process_5 = %u\n", reg_ctrl_val.bits.st_end_process_4);
	fprintf(stdout, "Status_end_process_6 = %u\n", reg_ctrl_val.bits.st_end_process_5);
	fprintf(stdout, "Status_end_process_7 = %u\n", reg_ctrl_val.bits.st_end_process_6);
	fprintf(stdout, "Status_end_process_8 = %u\n", reg_ctrl_val.bits.st_end_process_7);
}

void dump_memory(void *ptr, size_t len)
{
	int i;
	for (i=0; i<len; i++) {
		if (i%16 == 0) {
			fprintf(stdout, "\n");
		}
		fprintf(stdout, "%.2X ", ((unsigned char *)ptr)[i]);
	}
	fprintf(stdout, "\n");
}