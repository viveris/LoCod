# Validate time measurment from CPU
This small program is used to validate the measure of time from CPU with clock_gettime.

A GPIO is used to measure time with an oscilloscope and comparate with clock_gettime results.

## Build program

    aarch64-linux-gnu-gcc -o test_oscillo test_time_cpu_oscillo.c
