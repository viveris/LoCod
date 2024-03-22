#include "locod.h"

//======================================= Constant definitions ==========================================
//Registers
#define REG_VALUE(reg_index)		*((int*)reg_ptr + reg_index)

#ifdef DEBUG
#define DEBUG_PRINT(...) do{ printf(__VA_ARGS__ ); } while( 0 )
#else
#define DEBUG_PRINT(...) do{ } while ( 0 )
#endif //DEBUG

#if defined(LINUX)
	#define USLEEP(...) do{ usleep(__VA_ARGS__ ); } while( 0 )
#elif defined(BAREMETAL)
	#define USLEEP(...) do{ } while ( 0 )
#endif //LINUX or BAREMETAL 


//======================================= Types definition =======================================
typedef struct {
	int phy_addr;
	int param_len;
	int result_len;
	void *mem_ptr;
} accel_memory_t;


//========================================== Global variables ========================================
//Memory file descriptor
int fd = -1;

//Registers virtual pointer
void *reg_ptr = NULL;

//Shared memory for each accelerator
accel_memory_t *accel_memory;

int nb_accel = 0; //keeps track of what the number of accelerators
int next_addr = DMA_BASE_ADDR;  //keeps track of what the next base address should be when mapping memory
 

//=========================================== Function definitions ==============================================
int init_locod(int nb_acc)
{
	DEBUG_PRINT("%s - Initializing accelerator system...\n", __func__);

#if defined(LINUX)
	DEBUG_PRINT("%s - Open mem file descriptor... ", __func__);
	fd = open("/dev/mem", O_RDWR | O_SYNC);
	if (fd == -1)
	{
		DEBUG_PRINT("open fd failed\n\n");
		return -1;
	}
	DEBUG_PRINT("open fd succeed\n");

	DEBUG_PRINT("%s - Mmap control registers at address 0x%x... ", __func__, REG_AXI_ADDR);
	reg_ptr = mmap((NULL), (3*nb_acc+2)*sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fd, REG_AXI_ADDR);
	if (reg_ptr == MAP_FAILED)
	{
		DEBUG_PRINT("mmap failed\n\n");
		return -1;
	}
	DEBUG_PRINT("mmap succeed\n");
#elif defined(BAREMETAL)
	DEBUG_PRINT("%s - Initializing NG-Ultra board components... ", __func__);
	if (ngultra_init() != NGULTRA_INIT_OK) {
		DEBUG_PRINT("NG-Ultra init failed\n\n");
        return -1;
    }
	DEBUG_PRINT("NG-Ultra init succeed\n");

	reg_ptr = (void*)REG_AXI_ADDR;
#endif //LINUX or BAREMETAL 

	DEBUG_PRINT("%s - Reseting accelerators...\n", __func__);
	REG_VALUE(0) = 0xAAAAAAAA;
	DEBUG_PRINT("%s - Control reg out value during reset = 0x%x\n", __func__, REG_VALUE(0));
	USLEEP(0.1);
	REG_VALUE(0) = 0x0;
	DEBUG_PRINT("%s - Control reg out value after reset = 0x%x\n", __func__, REG_VALUE(0));

	DEBUG_PRINT("%s - Initializing accel_memory structure... ", __func__);
	nb_accel = nb_acc;
	accel_memory = malloc(nb_accel * sizeof(accel_memory_t));
	if (accel_memory == NULL)
	{
		DEBUG_PRINT("malloc failed\n\n");
		return -1;
	}
	DEBUG_PRINT("malloc succeed\n\n");

	return 0;
}


int init_accelerator_memory(int param_len, int result_len, int accel)
{	
	DEBUG_PRINT("%s - Initializing accelerator %d memory...\n", __func__, accel);
	accel_memory[accel].phy_addr = next_addr;

	if (accel_memory[accel].phy_addr % 4096)
	{
		accel_memory[accel].phy_addr += 4096 - (accel_memory[accel].phy_addr % 4096);
	}

	accel_memory[accel].param_len = param_len;
	accel_memory[accel].result_len = result_len;

	next_addr = accel_memory[accel].phy_addr + param_len + result_len;

	DEBUG_PRINT("%s - Param data physical address : 0x%x\n", __func__, accel_memory[accel].phy_addr);
	DEBUG_PRINT("%s - Result data physical address : 0x%x\n", __func__, accel_memory[accel].phy_addr + accel_memory[accel].param_len);

#if defined(LINUX)
	DEBUG_PRINT("%s - Mmap accelerator %d memory at address 0x%x... ", __func__, accel, accel_memory[accel].phy_addr);
	accel_memory[accel].mem_ptr = mmap((NULL), param_len + result_len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, accel_memory[accel].phy_addr);
	if (accel_memory[accel].mem_ptr == MAP_FAILED)
	{
		DEBUG_PRINT("mmap failed\n\n");
		return -1;
	}
	DEBUG_PRINT("mmap succeed\n\n");
#elif defined(BAREMETAL)
	accel_memory[accel].mem_ptr = (void*)accel_memory[accel].phy_addr;
	DEBUG_PRINT("\n");
#endif //LINUX or BAREMETAL

	return 0;
}


int cp_param_and_result_to_accel_memory(void *param_addr, void *result_addr, int accel)
{
	DEBUG_PRINT("%s - Copying param and result data to accelerator %d memory... ", __func__, accel);
	if (memcpy(accel_memory[accel].mem_ptr, param_addr, accel_memory[accel].param_len) == NULL)
	{
		DEBUG_PRINT("copy failed\n\n");
		return -1;
	}
	if (memcpy(accel_memory[accel].mem_ptr + accel_memory[accel].param_len, result_addr, accel_memory[accel].result_len) == NULL)
	{
		DEBUG_PRINT("copy failed\n\n");
		return -1;
	}
	DEBUG_PRINT("copy succeed\n\n");

	return 0;
}


int cp_result_from_accel_memory(void *result_addr, int accel)
{
	DEBUG_PRINT("%s - Copying result data from accelerator %d memory... ", __func__, accel);
	if (memcpy(result_addr, accel_memory[accel].mem_ptr + accel_memory[accel].param_len, accel_memory[accel].result_len) == NULL)
	{
		DEBUG_PRINT("copy failed\n\n");
		return -1;
	}
	DEBUG_PRINT("copy succeed\n\n");

	return 0;
}


int start_accelerator(int accel)
{
	DEBUG_PRINT("%s - Starting accelerator %d...\n", __func__, accel);

	DEBUG_PRINT("%s - Copying addresses of param and result to accelerator registers\n", __func__);
	REG_VALUE((2*accel) + 1) = (int)accel_memory[accel].phy_addr;
	REG_VALUE((2*accel) + 2) = (int)accel_memory[accel].phy_addr + accel_memory[accel].param_len;
	DEBUG_PRINT("%s - param register at 0x%x\n", __func__, REG_VALUE((2*accel) + 1));
	DEBUG_PRINT("%s - result register at 0x%x\n", __func__, REG_VALUE((2*accel) + 2));

	DEBUG_PRINT("%s - Setting start bit\n", __func__);
	REG_VALUE(0) |= 1 << 2*accel;
	DEBUG_PRINT("%s - Control reg out value after high = 0x%x\n", __func__, REG_VALUE(0));
	USLEEP(0.1);
	REG_VALUE(0) &= ~(1 << 2*accel);
	DEBUG_PRINT("%s - Control reg out value after low = 0x%x\n", __func__, REG_VALUE(0));
	DEBUG_PRINT("%s - Control reg in value after low = 0x%x\n\n", __func__, REG_VALUE(2*nb_accel+1));

	return 0;
}


int wait_accelerator(void *result_addr, int accel)
{
	DEBUG_PRINT("%s - Waiting accelerator %d to finish...\n", __func__ , accel);
	DEBUG_PRINT("%s - Control reg in value on entrance = 0x%x\n", __func__, REG_VALUE(2*nb_accel+1));	

	while ((REG_VALUE(2*nb_accel+1) & (1<<accel)) == 0) {
		USLEEP(POLL_PERIOD_US);
	}

	return cp_result_from_accel_memory(result_addr, accel);
}


int get_time_ns_FPGA(int accel)
{
	DEBUG_PRINT("%s - Getting duration value from accelerator %d...\n\n", __func__ , accel);
	int fpga_cnt_latched = REG_VALUE(2*nb_accel+1+accel+1);
	return fpga_cnt_latched * (1000000000 / FPGA_FREQ_HZ);
}


int deinit_locod(void)
{
	DEBUG_PRINT("%s - Deinitializing accelerator system...\n", __func__);

#if defined(LINUX)
	for (int accel = 0 ; accel < nb_accel ; accel++)
	{
		DEBUG_PRINT("%s - Munmap accelerator %d memory at address 0x%x... ", __func__, accel, accel_memory[accel].phy_addr);
		if (munmap(accel_memory[accel].mem_ptr, accel_memory[accel].param_len + accel_memory[accel].result_len) == -1)
		{
			DEBUG_PRINT("munmap failed\n\n");
			return -1;
		}
		DEBUG_PRINT("munmap succeed\n");
	}

	DEBUG_PRINT("%s - Munmap control registers at address 0x%x... ", __func__, REG_AXI_ADDR);
	if (munmap(reg_ptr, 32*sizeof(int)) == -1)
	{
		DEBUG_PRINT("munmap failed\n\n");
		return -1;
	}
	DEBUG_PRINT("munmap succeed\n");

	DEBUG_PRINT("%s - Closing mem file descriptor...\n\n", __func__);
	close(fd);
#elif defined(BAREMETAL)
	DEBUG_PRINT("\n");
#endif //LINUX or BAREMETAL

	free(accel_memory);

	return 0;
}