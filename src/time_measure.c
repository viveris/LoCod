#include "time_measure.h"
#include "map_memory.h"

/**
 * @brief Calculate a delta between two timespec.
 * @param[in] t_start Pointer to the start time.
 * @param[in] t_end Pointer to the end time.
 */
struct timespec diff_ts(struct timespec *t_start, struct timespec *t_end)
{
	struct timespec t_diff = { 0 };

	if ((t_end->tv_nsec - t_start->tv_nsec) < 0 /* ns */) {
		t_diff.tv_sec = t_end->tv_sec - t_start->tv_sec - 1;
		t_diff.tv_nsec = 1000000000 /* ns */ + t_end->tv_nsec - t_start->tv_nsec;
	} else {
		t_diff.tv_sec = t_end->tv_sec - t_start->tv_sec;
		t_diff.tv_nsec = t_end->tv_nsec - t_start->tv_nsec;
	}

	return t_diff;
}

/**
 * @brief Add two timespec.
 * @param[in] t1 Pointer to a first timespec.
 * @param[in] t2 Pointer to a second timespec.
 */
struct timespec add_ts(struct timespec *t1, struct timespec *t2)
{
	struct timespec t = { 0 };

	if ((t1->tv_nsec + t2->tv_nsec) > 1000000000 /* ns */) {
		t.tv_sec = t1->tv_sec + t2->tv_sec + 1;
		t.tv_nsec = t1->tv_nsec + t2->tv_nsec - 1000000000 /* ns */;
	} else {
		t.tv_sec = t1->tv_sec + t2->tv_sec;
		t.tv_nsec = t1->tv_nsec + t2->tv_nsec;
	}

	return t;
}

#define s_to_ns 1000000000
uint32_t get_time_ns_FPGA()
{
	uint32_t fpga_cnt = get_reg_dur();
	return fpga_cnt * (s_to_ns / FPGA_FREQ_Hz);
}
