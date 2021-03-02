#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#define GPIO_TIME "374"

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



int main()
{
	struct timespec t1, t2;

	int fd = open("/sys/class/gpio/export", O_WRONLY);
	if (fd == -1) {
		fprintf(stderr, "Unable to open /sys/class/gpio/export");
		exit(1);
	}

	if (write(fd, GPIO_TIME, strlen(GPIO_TIME)) != strlen(GPIO_TIME)) {
		fprintf(stderr, "Error writing to /sys/class/gpio/export");
		exit(1);
	}

	close(fd);

	// Set the pin to be an output by writing "out" to /sys/class/gpio/gpio24/direction

	fd = open("/sys/class/gpio/gpio"GPIO_TIME"/direction", O_WRONLY);
	if (fd == -1) {
		fprintf(stderr, "Unable to open /sys/class/gpio/gpio"GPIO_TIME"/direction");
		exit(1);
	}

	if (write(fd, "out", 3) != 3) {
		fprintf(stderr, "Error writing to /sys/class/gpio/gpio"GPIO_TIME"/direction");
		exit(1);
	}

	close(fd);

	fd = open("/sys/class/gpio/gpio"GPIO_TIME"/value", O_WRONLY);
	if (fd == -1) {
		fprintf(stderr, "Unable to open /sys/class/gpio/gpio"GPIO_TIME"/value");
		exit(1);
	}

	for (int i=0; i<60000; i++) {

		if (write(fd, "1", 1) != 1) {
			fprintf(stderr, "Error writing to /sys/class/gpio/gpio"GPIO_TIME"/value");
			exit(1);
		}
		clock_gettime(CLOCK_MONOTONIC, &t1);
		usleep(50);

		if (write(fd, "0", 1) != 1) {
			fprintf(stderr, "Error writing to /sys/class/gpio/gpio"GPIO_TIME"/value");
			exit(1);
		}
		usleep(100);
		clock_gettime(CLOCK_MONOTONIC, &t2);
		struct timespec t = diff_ts(&t1, &t2);
		fprintf(stdout, "Time = %ld s %ld ns\n", t.tv_sec, t.tv_nsec);
	}

	close(fd);

	// Unexport the pin by writing to /sys/class/gpio/unexport

	fd = open("/sys/class/gpio/unexport", O_WRONLY);
	if (fd == -1) {
		fprintf(stderr, "Unable to open /sys/class/gpio/unexport");
		exit(1);
	}

	if (write(fd, GPIO_TIME, 3) != 2) {
		fprintf(stderr, "Error writing to /sys/class/gpio/unexport");
		exit(1);
	}

	close(fd);

	// And exit
	return 0;
}
