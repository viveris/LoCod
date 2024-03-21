# Locod

## Overview
Locod is a hardware/software co-design tool designed to simplify the development of applications for system-on-chip (SoC) devices.

It enables the implementation and testing of hybrid applications, i.e. with a CPU component and an FPGA component. For example, a classic CPU program where we want to execute a processing function in the FPGA.

The Locod tool then provides a simple API for executing one or more functions in the FPGA, monitoring its execution and retrieving its outputs and performance metrics.

<br>

## Support
Three target boards are currently supported by the Locod tool:
|      Target board      | Locod compilation | Test on board |
| ---------------------- |:-----------------:|:-------------:|
| Avnet Ultra96          |       OK          |      OK       |
| Enclustra Mercury+ XU1 |       OK          |      OK       |
| NanoXplore NG-Ultra    |       OK          |      KO       |

 <br>

## Requierments

The Locod tool requires different development environments to operate. These dependencies are checked each time the tool is launched. If one of these dependencies is missing, Locod will not work.

Most of these environments/dependencies have been dockerised to facilitate porting to different machines.

Here are the Locod dependencies:
- the Panda-Bambu docker, which includes the Panda-Bambu tool for converting functions into HDL code: http://gitlab-ci.b2i-toulouse.prive/CNES-Embedded/locod-study/locod-panda-docker

- the Ultra96 SDK docker for compiling on Ultra96: http://gitlab-ci.b2i-toulouse.prive/CNES-Embedded/locod-study/locod-sdk-ultra96

- the Enclustra SDK docker for compiling on Enclustra: http://gitlab-ci.b2i-toulouse.prive/CNES-Embedded/locod-study/locod-sdk-enclustra

- the NG-Ultra SDK docker for compiling on NG-Ultra: TODO: create a repository for the NG-Ultra BSP

- Vivado 2022.1 and with a license for all the devices (mandatory to synthesize FPGA design for the Enclustra board)

<br>

## Install

The Locod tool does not require any specific installation. The only requirement is that the necessary dockers and tools are present on the system, and that their names match those in the main Locod script:

```console
# Docker images
PANDA_DOCKER_IMG=panda-bambu:9.8.0
ULTRA96_SDK_DOCKER_IMG=sdk-ultra96:1.0
ENCLUSTRA_SDK_DOCKER_IMG=sdk-enclustra:1.0
NG_ULTRA_SDK_DOCKER_IMG=sdk-ngultra:1.0
NX_DOCKER_IMG=nx-tools:2.0
```

<br>

## Usage

The Locod tool is based on code developed in C. It is used via an API providing various functions for executing code in the FPGA and monitoring its operation.

Here are the various functions available:
```c
int init_locod(int nb_acc);

//Locod framework initialization function. It must be called before all other functions. The **nb_acc** parameter defines the number of functions to be executed in the FPGA.
```

```c
int init_accelerator_memory(struct fpga_param param, struct fpga_param result, int accel);

//Memory initialization function for the accelerator n. It allocates the space needed to store input data and results in the area shared with the FPGA.
```


```c
int cp_param_and_result_to_accel_memory(struct fpga_param param, struct fpga_param result, int accel);

//Function that copies parameters and results to the n accelerator's memory area.
```

```c
int cp_result_from_accel_memory(struct fpga_param result, int accel);

//Function that retrieves results from the n accelerator's memory area.
```

```c
int start_accelerator(int accel);

//Function that starts execution of accelerator n.
```

```c
int wait_accelerator(struct fpga_param result, int accel);

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

### Simple example


<br>