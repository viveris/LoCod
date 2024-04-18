# LoCod

## Overview
LoCod is a hardware/software co-design tool designed to simplify the development of applications for system-on-chip (SoC) devices.

It facilitates the implementation and testing of hybrid applications, i.e. with a CPU component and an FPGA component. For example, a classic CPU program where we want to execute a processing function in the FPGA.

The LoCod tool then provides a simple C code API for executing one or more functions in the SoC FPGA, known as hardware accelerators, then monitoring its execution and retrieving its outputs and some performance metrics.

LoCod uses the [Panda-Bambu](https://github.com/ferrandi/PandA-bambu) tool to convert the C code of accelerated functions into HDL language for bitstream generation. An entire HDL architecture is then automatically generated to easily interface these accelerators with the CPU, so that the use of accelerated functions is as similar as possible to conventional CPU execution. More details on how the LoCod tool works can be found in [locod_operation.md](doc/locod_operation.md) file.

<br>

## Support
Three target boards are currently supported by the LoCod tool, 2 from Xilinx and 1 from NanoXplore:

|      Target board      | LoCod compilation | Test on board |
| ---------------------- |:-----------------:|:-------------:|
| Avnet Ultra96          |       OK          |      OK       |
| Enclustra Mercury+ XU1 |       OK          |      OK       |
| NanoXplore NG-Ultra    |       OK          |      KO       |

The [add_new_target.md](doc/add_new_target.md) file describes the main steps to add a new target to the LoCod project.

 <br>

## Requierments

The LoCod tool requires different development environments to operate.

Most of these environments/dependencies have been dockerised to facilitate porting to different machines. The various Dockers are available as submodules in the [submodules/docker/](submodules/docker/) folder.

These dependencies are checked each time the tool is launched. Depending on the target selected, only the dependencies required for it are checked. So if the LoCod tool is used exclusively for a Xilinx target, the NanoXplore tools and docker are not required. However, if one of these dependencies is missing, LoCod script will stop.

Here are the LoCod common dependencies:
- the Panda-Bambu docker, which includes the Panda-Bambu tool for converting functions into HDL code: https://github.com/viveris/LoCod-docker-PandA

Here are the LoCod Xilinx dependencies:
- Vivado 2022.1 and with working ML Entreprise license (mandatory to synthesize FPGA design for the Enclustra board)
- the Ultra96 SDK docker for compiling on Ultra96: https://github.com/viveris/LoCod-docker-sdk-ultra96
- the Enclustra SDK docker for compiling on Enclustra: https://github.com/viveris/LoCod-docker-sdk-enclustra

Here are the LoCod NanoXplore dependencies:
- the NanoXplore docker with the NX Design Suite to synthesize FPGA design for NanoXplore targets: https://github.com/viveris/LoCod-docker-nanoxplore
- the NG-Ultra SDK docker for compiling on NG-Ultra: https://github.com/viveris/LoCod-docker-sdk-ngultra

<br>

## Install

The LoCod tool does not require any specific installation. The only requirement is that the necessary dockers and tools are present on the system, and that their names match those in the main LoCod script:

```console
# Docker images
PANDA_DOCKER_IMG=panda-bambu:9.8.0
ULTRA96_SDK_DOCKER_IMG=sdk-ultra96:1.0
ENCLUSTRA_SDK_DOCKER_IMG=sdk-enclustra:1.0
NG_ULTRA_SDK_DOCKER_IMG=sdk-ngultra:1.0
NX_DOCKER_IMG=nx-tools:2.0

#Impulse license
NX_HOSTNAME=localhost.localdomain
NX_MAC_ADDR=86:8a:dd:8d:51:a8
```

<br>

## Usage

To illustrate how the LoCod tool works, let's take a simple example. Let's say, for example, that we wish to execute 2 functions in the FPGA:
- the first takes two integers as input and multiplies them

- the second takes an array of floats as input and returns 2 float results, the first being the sum of all inputs and the second the subtraction of all inputs

<br>

### Input code syntax

**Functions to be executed in the FPGA must be void and have two structure pointers as arguments**. The first must be named **\*param** and point to the input data, and the second must be named **\*result** and point to the output data.

```c
void fpga_function(struct type_param *param, struct type_result *result) {
    ...
}
```
Data passed through these pointers must be stored statically in data structures. The param and result pointers must therefore point to structures only, and not to common types as int or float. In addition, there must be no pointers in the structures used. When processing arrays, these must be of fixed size and statically declared.

For our first example function, here are the param and result structures:
```c
struct param_acc0 {
    int a;
    int b;
};

struct result_acc0 {
    int a;
};

void acc0(struct param_acc0 *param, struct result_acc0 *result) {
    result->a = param->a * param->b;
}
```

And for our second example function:
```c
struct param_acc1 {
    float a[SIZE];
};

struct result_acc1 {
    float a;
    float b;
};

void acc1(struct param_acc1 *param, struct result_acc1 *result) {
    int i = 0;
    result->a = 0.0;
    result->b = 0.0;

    for (i = 0; i < SIZE; i++) {
        result->a += param->a[i];
        result->b -= param->a[i];
    }
}
```

Now that we've defined the functions we want to run in the FPGA, let's take a look at how to run them.

First of all, we need to include the **locod.h** header, which contains all the declarations required to use the LoCod API.

Also note that you need to add compiler directives **#ifndef LOCOD_FPGA** to limit the C file to functions to be executed in the FPGA when this define exists. The define LOCOD_FPGA is defined when the C file is passed to the tool that converts C functions to VHDL. However, it only needs the functions and not everything else, hence the addition of #ifndef to limit the C file to functions. In our example, we will add these directives around the "locod.h" include and around the main() function.

Next, calling up functions in the FPGA or CPU is done through macros in the C code to tell the LoCod system where that this or that function must be executed. Apart from these macros, functions can also be called in the normal way to be executed in the CPU.

Before calling these macros to execute functions in the FPGA, the LoCod system initialization function **init_locod** must be called. This function takes as arguments the number of hardware accelerators to be instantiated in the FPGA, i.e. the number of functions to be executed in the FPGA. This number can be greater than the number of functions finally instantiated, but not less, otherwise it won't work.

For our example, we'll start by defining our main function, setting the variables we'll use with our hardware accelerators. We then call the LoCod initialization function with an accelerator number equal to 2:
```c
#ifndef LOCOD_FPGA
#include "locod.h"
#endif //LOCOD_FPGA

#define SIZE 20

#ifndef LOCOD_FPGA
int main(void) {
    //Variables
    struct param_acc0 param_acc_0 = {.a = 3, .b = 7};
    struct result_acc0 result_acc_0 = {.a = 0};
    struct param_acc1 param_acc_1;
    for (int i = 0; i < SIZE; i++) {
        param_acc_1.a[i] = i;
    }
    struct result_acc1 result_acc_1 = {.a = 0, .b = 0};

    //LoCod initialization
    init_locod(2);
```

Two macros CPU and FPGA are then available to launch the various functions either in CPU or FPGA :
-   The **FPGA macro** is used to launch a function in the FPGA. It takes 4 parameters as input:
    - the name of the function to be executed in the FPGA
    - a pointer of param argument type for the input data 
    - a pointer of result argument type for the output data
    - the number of the accelerator that will execute the function. It is this number that will later be used to drive the FPGA accelerator and retrieve the results once execution is complete.

    ```c
    FPGA(fct, param_ptr, result_ptr, accel)
    ```

-   The **CPU macro** is used to launch a function in the CPU. It takes 3 parameters as input:
    - the name of the function to be executed in the CPU
    - a pointer of param argument type for the input data 
    - a pointer of result argument type for the output data

    This macro does the same thing than launching the function the normal way in our code.

    However, the CPU macro allows to check the function prototype used to match that required prototype for an FPGA function. This makes it easier to change between CPU or FPGA execution afterwards.

    ```c
    CPU(fct, param_ptr, result_ptr)
    ```

We can then run our two example functions in the FPGA (which is what we're most interested in here). The two FPGA functions are called one after the other. The two functions will run in parallel in the FPGA:
```c
    //Launching acc1 and acc2 function in the FPGA
    FPGA(acc0, &param_acc_0, &result_acc_0, 0);
    FPGA(acc1, &param_acc_1, &result_acc_1, 1);
```

Results cannot be retrieved directly. In other words, the result pointer is not directly modified with the execution result. To retrieve the result, we need to call another function of the API, **wait_accelerator**. This function takes two arguments. The first is a pointer of result argument type (the same pointer that the one passed to the FPGA macro), and the second is the accelerator number from which we want to restrive the results (here also the same number than the one passed to the FPGA macro). This functions waits until the accelerator has finished, then copies the accelerator outputs to the result pointer.

Here is the code to retreve results from our two hardware accelerators example:
```c
    //Retreive outputs
    wait_accelerator(&result_acc_0, 0);
    wait_accelerator(&result_acc_1, 1);

    //Print results
    printf("Acc 0 result : %d * %d = %d\n", param_acc_0.a, param_acc_0.b, result_acc_0.a);
    printf("Acc 1 result : sum of input values = %f, substraction of input values = %f\n", result_acc_1.a, result_acc_1.b);
```

It is also possible to retrieve the execution time of the function in the FPGA with the **get_time_ns_FPGA** function. This returns the execution time of the selected accelerator in nanoseconds. Be careful, however, if the execution time is too long to fit into an entie, then the time displayed may be wrong.

```c
    //Print execution time of the 2 accelerators
    printf("Acc 0 execution time = %d ns\n", get_time_ns_FPGA(0));
    printf("Acc 1 execution time = %d ns\n", get_time_ns_FPGA(0));
```

Finally, the locod can be de-initialized with the **deinit_locod** function, to free up memory resources taken by the system:
```c
    //LoCod de-initialization
    deinit_locod();

     return 0;
} //End main()
#endif //LOCOD_FPGA
```
<br>

### Launching the LoCod tool

Now that the C input code has been developed with the correct syntax for launching code in the FPGA, it needs to be passed to the LoCod tool to generate :
- CPU executable
- FPGA bitstream

The tool is launched with a **locod.sh** bash script, which calls the various tools one after the other to compile the C code, convert the functions passed in the FPGA() macros into VHDL and generate the FPGA VHDL system including the hardwares accelerators corresponding to these functions.

The **locod.sh** script must be is used as follows:
```console
./locod.sh  
    [ -t | --target ] < target board : enclustra, ultra96, ngultra >
    [ -f | --file ] < main C file >
    [ --no-hard ] < don't generate bitstream >
    [ --no-soft ] < don't generate executable >
    [ -h | --help  ]
```

The execution of the LoCod tool can then be monitored with logs in the console.

Depending on the target (--target) selected, the various Dockers and tools requiered must be available on the machine. If one is missing, the script will stop.

For our example, let's say we want to test it on an Enclustra board. Assuming the dockers are present, we still need to source the Vivado environment script required for VHDL synthesis. A valid Vivado license is also required, as the Enclustra embeds a Zynq Ultrascale+ XCZU6EG not available with the free version.

We will then run the following commands:
```console
source <Vivado 2022.1 install directory>/settings64.sh
./locod.sh -t enclustra -f demo/example_readme/main.c
```

Once the locod has been compiled, our two outputs **fpga.bit** and **locod-cpu** can be found in the **locod-output/** folder. The name of outputs may vary depending on the selected target.

We can then take these files, send them to the enclustra board, flash the FPGA, run the locod-cpu executable, and finally observe the results in the console:
```console
fpgautil -b fpga.bit
...

./locod-cpu
...
Acc 0 result : 3 * 7 = 21
Acc 1 result : sum of input values = 190.000000, substraction of input values = -190.000000
```

As we can see, we get good results.

<br>