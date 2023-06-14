#ifndef _TIME_MEASURE_H_
#define _TIME_MEASURE_H_

#include <time.h>
#include <stdint.h>

struct timespec diff_ts(struct timespec *t_start, struct timespec *t_end);
struct timespec add_ts(struct timespec *t1, struct timespec *t2);
uint32_t get_time_ns_FPGA();


#endif /* _TIME_MEASURE_H_ */
