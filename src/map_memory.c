#include <string.h>

#include "map_memory.h"

/* Registers ADDR */
#define REG_CTRL                 0xA0010000 /* Control and status register, bit definition below */
#define REG_CTRL_BIT_START       1
union reg_ctrl {
	unsigned int reg;
	struct {
		unsigned int ctrl_start       : 1;
		unsigned int ctrl_reset       : 1;
		unsigned int ctrl_autorestart : 1;
		unsigned int st_reset         : 1;
		unsigned int st_start         : 1;
		unsigned int /* PADDING */    : 3;
		unsigned int st_run           : 1;
		unsigned int st_end           : 1;
	} bits;
};

#define REG_IN_1                 0xA0020000
#define REG_IN_2                 0xA0030000
#define REG_OUT                  0xA0040000
#define REG_DURATION             0xA0050000
#define REG_DURATION_LATCHED     0xA0000000
#define REG_ITER                 0xA0060000

#define REG_MODE                 0x4

#define POLL_PERIOD_US           1

struct mmap_area {
	off_t phy_addr;
	off_t param_offset;
	off_t result_offset;
};
struct mmap_area mmapped = { 0 };

/* File descriptor for /dev/mem open */
static int fd = -1;

union  reg_ctrl *ptr_reg_ctrl_st     = NULL;
static uint32_t *ptr_reg_in_1        = NULL;
static uint32_t *ptr_reg_in_2        = NULL;
static uint32_t *ptr_reg_out_1       = NULL;
static uint32_t *ptr_reg_dur         = NULL;
static uint32_t *ptr_reg_dur_latched = NULL;
static uint32_t *ptr_reg_iter        = NULL;

#ifdef TIME_MEASURE
struct timespec ts_acc_start = { 0 };
struct timespec ts_acc_end = { 0 };
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

void init_accelerator_memory(off_t phy_addr,
                             off_t param_offset,
                             off_t result_offset)
{
	/* TODO debug */
	fprintf(stdout,
	        "Init accelerator memory : Addr=0x%ld, param off=%ld, result off=%ld\n",
	        phy_addr, param_offset, result_offset);

	mmapped.phy_addr = phy_addr;
	mmapped.param_offset = param_offset;
	mmapped.result_offset = result_offset;
}

int unmap_phys_addr(void *virt_ptr, size_t len)
{
	if ( munmap(virt_ptr, sizeof(int)) ) {
		perror("Cannot unmap");
		return -1;
	}

	return 0;
}

int init_dma()
{
	if (fd != -1) {
		fprintf(stdout, "Already initialized \n");
		return -1;
	}

	if ( open_devmem() ) {
		return -1;
	}

	/* Map all registers */
	if ( map_phys_addr(REG_CTRL, sizeof(uint32_t), (void **)&ptr_reg_ctrl_st) ) {
		fprintf(stdout, "Mmap for control register failed\n");
		return -1;
	}

	if ( map_phys_addr(REG_IN_1, sizeof(uint32_t), (void **)&ptr_reg_in_1) ) {
		fprintf(stdout, "Mmap for input register 1 failed\n");
		return -1;
	}

	if ( map_phys_addr(REG_IN_2, sizeof(uint32_t), (void **)&ptr_reg_in_2) ) {
		fprintf(stdout, "Mmap for input register 2 failed\n");
		return -1;
	}

	if ( map_phys_addr(REG_OUT, sizeof(uint32_t), (void **)&ptr_reg_out_1) ) {
		fprintf(stdout, "Mmap for output register 1 failed\n");
		return -1;
	}

	if ( map_phys_addr(REG_DURATION, sizeof(uint32_t), (void **)&ptr_reg_dur) ) {
		fprintf(stdout, "Mmap for duration register failed\n");
		return -1;
	}

	if ( map_phys_addr(REG_DURATION, sizeof(uint32_t), (void **)&ptr_reg_dur_latched) ) {
		fprintf(stdout, "Mmap for latched duration register failed\n");
		return -1;
	}

	if ( map_phys_addr(REG_ITER, sizeof(uint32_t), (void **)&ptr_reg_iter) ) {
		fprintf(stdout, "Mmap for iteration register failed\n");
		return -1;
	}


	/* Configure register input / output mode */
	*(ptr_reg_dur + REG_MODE) = 0xFFFFFFFF;
	*(ptr_reg_dur_latched + REG_MODE) = 0xFFFFFFFF;
	*(ptr_reg_iter + REG_MODE) = 0xFFFFFFFF;
	(ptr_reg_ctrl_st + REG_MODE)->reg = 0x0000FF00;

	fprintf(stdout, "Mapping and register mode is initialized\n");

	return 0;
}

int cp_param_to_fpga(struct fpga_param *param)
{
#ifdef TIME_MEASURE
	struct timespec t1;
	struct timespec t2;
	if (clock_gettime(CLOCK_MONOTONIC, &t1)) {
		perror("Cannot get time\n");
		return -1;
	}
#endif /* TIME_MEASURE */

	void *mmap_ptr = mmap(NULL,
	                      param->len,
	                      PROT_WRITE | PROT_READ,
	                      MAP_SHARED,
	                      fd,
	                      MEM_DMA_BASE + mmapped.param_offset);
	if (mmap_ptr == MAP_FAILED) {
		perror("Cannot mmap");
		return -1;
	}
	memcpy(mmap_ptr, param->p, param->len);
	munmap(mmap_ptr, param->len);

#ifdef TIME_MEASURE
	if (clock_gettime(CLOCK_MONOTONIC, &t2)) {
		perror("Cannot get time\n");
		return -1;
	}
	struct timespec t_memcpy = diff_ts(&t1, &t2);
	fprintf(stdout,
	        "Copy parameters from CPU to FPGA: %zu "
	        "bytes in %zu s and %zu ns\n",
	        param->len, t_memcpy.tv_sec, t_memcpy.tv_nsec);
#endif /* TIME_MEASURE */

	return 0;
}

int cp_result_from_fpga(struct fpga_param *result)
{
#ifdef TIME_MEASURE
	struct timespec t1;
	struct timespec t2;
	if (clock_gettime(CLOCK_MONOTONIC, &t1)) {
		perror("Cannot get time\n");
		return -1;
	}
#endif /* TIME_MEASURE */

	void *mmap_ptr = mmap(NULL,
	                      mmapped.param_offset + result->len,
	                      PROT_WRITE | PROT_READ,
	                      MAP_SHARED,
	                      fd,
	                      MEM_DMA_BASE);
	if (mmap_ptr == MAP_FAILED) {
		perror("Cannot mmap");
		return -1;
	}
	memcpy(result->p, mmap_ptr + mmapped.result_offset, result->len);
	munmap(mmap_ptr, result->len);

#ifdef TIME_MEASURE
	if (clock_gettime(CLOCK_MONOTONIC, &t2)) {
		perror("Cannot get time\n");
		return -1;
	}
	struct timespec t_memcpy = diff_ts(&t1, &t2);
	fprintf(stdout,
	        "Copy result from FPGA to CPU: %zu "
	        "bytes in %zu s and %zu ns\n",
	        result->len, t_memcpy.tv_sec, t_memcpy.tv_nsec);
#endif /* TIME_MEASURE */

	return 0;

}

int start_accelerator()
{
	/* TODO debug */
	fprintf(stdout, "Starting processing of accelerator\n");

	if (ptr_reg_ctrl_st == NULL) {
		fprintf(stdout, "%s not initialized\n", __func__);
		return -1;
	}

	ptr_reg_ctrl_st->bits.ctrl_start = 1;

#ifdef TIME_MEASURE
	if (clock_gettime(CLOCK_MONOTONIC, &ts_acc_start)) {
		perror("Cannot get time\n");
		return -1;
	}
#endif /* TIME_MEASURE */

	return 0;
}

int wait_accelerator(struct fpga_param *result)
{
	if (ptr_reg_ctrl_st == NULL) {
		fprintf(stdout, "%s not initialized\n", __func__);
		return -1;
	}

	while (ptr_reg_ctrl_st->bits.st_end != 1) {
		usleep(POLL_PERIOD_US);
	}

	cp_result_from_fpga(result);

#ifdef TIME_MEASURE
	if (clock_gettime(CLOCK_MONOTONIC, &ts_acc_end)) {
		perror("Cannot get time\n");
		return -1;
	}

	struct timespec t_processing = diff_ts(&ts_acc_start, &ts_acc_end);
	fprintf(stdout, "Processing time: %zu s and %zu ns\n",
	        t_processing.tv_sec, t_processing.tv_nsec);
	fprintf(stdout, "Processing time from FPGA: %u ns\n", get_time_ns_FPGA());
#endif /* TIME_MEASURE */

	return 0;
}

uint32_t get_reg_dur()
{
	return *ptr_reg_dur_latched;
}

void set_in_1(uint32_t value)
{
	*ptr_reg_in_1 = value;
}

void set_in_2(uint32_t value)
{
	*ptr_reg_in_2 = value;
}

void set_out_1(uint32_t value)
{
	*ptr_reg_out_1 = value;
}

void print_ctrl_st_reg(void)
{
	union reg_ctrl reg_ctrl_val = { 0 };
	reg_ctrl_val.reg = ptr_reg_ctrl_st->reg ;

	fprintf(stdout, "Ctrl / Status register value : 0x%X\n", reg_ctrl_val.reg);
	fprintf(stdout, "Start bit = %u\n", reg_ctrl_val.bits.ctrl_start);
	fprintf(stdout, "Reset bit = %u\n", reg_ctrl_val.bits.ctrl_reset);
	fprintf(stdout, "Autorestart bit = %u\n", reg_ctrl_val.bits.ctrl_autorestart);
	fprintf(stdout, "Status reset bit = %u\n", reg_ctrl_val.bits.st_reset);
	fprintf(stdout, "Status start bit = %u\n", reg_ctrl_val.bits.st_start);
	fprintf(stdout, "Status running bit = %u\n", reg_ctrl_val.bits.st_run);
	fprintf(stdout, "Status end bit = %u\n", reg_ctrl_val.bits.st_end);
}

