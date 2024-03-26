The Locod tool is based on code developed in C. It is used via an API providing various functions for executing code in the FPGA and monitoring its operation.

Here are the various functions available:
```c
int init_locod(int nb_acc);

//Locod framework initialization function. It must be called before all other functions. The nb_acc parameter defines the number of functions to be executed in the FPGA.
```

```c
int init_accelerator_memory(int param_len, int result_len, int accel);

//Memory initialization function for the accelerator n. It allocates the space needed to store input data and results in the area shared with the FPGA.
```


```c
int cp_param_and_result_to_accel_memory(void *param_addr, void *result_addr, int accel);

//Function that copies parameters and results to the n accelerator's memory area.
```

```c
int cp_result_from_accel_memory(void *result_addr, int accel);

//Function that retrieves results from the n accelerator's memory area.
```

```c
int start_accelerator(int accel);

//Function that starts execution of accelerator n.
```

```c
int wait_accelerator(void *result_addr, int accel);

//Function that waits for the accelerator n to finish executing, and then retrieves the results from the shared memory area.
```

```c
int get_time_ns_FPGA(int accel);

//Debug function that recovers the execution time of a accelerator in nanoseconds. Accelerator performance can then be compared with CPU execution.
```

```c
int deinit_locod(void);

//Locod framework deinitialization function.
```