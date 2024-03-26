# Locod

## Overview
Locod is a hardware/software co-design tool designed to simplify the development of applications for system-on-chip (SoC) devices.

It enables the implementation and testing of hybrid applications, i.e. with a CPU component and an FPGA component. For example, a classic CPU program where we want to execute a processing function in the FPGA.

The Locod tool then provides a simple API for executing one or more functions in the FPGA, known as hardware accelerators, then monitoring its execution and retrieving its outputs and some performance metrics.

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

To illustrate how the Locod tool works, let's take a simple example. Let's say, for example, that we wish to execute 2 functions in the FPGA:
- the first takes two integers as input and multiplies them

- the second takes an array of floats as input and returns 2 float results, the first being the sum of all inputs and the second the subtraction of all inputs

<br>

### Input code syntax

**Functions to be executed in the FPGA must be void and have two pointers as arguments**. The first must be named **\*param** and point to the input data, and the second must be named **\*result** and point to the output data.

```c
void fpga_function(type_param *param, type_result *result) {
    ...
}
```

The param and result pointers can then be simple types such as int, float, etc., if the function process is simple. If the function is more complex, with more inputs and outputs, then simply define data structures and pass these structures as pointer types for the function.

For our first example function, here are the param and result structures:
```c
struct param_acc0 {
    int a;
    int b;
};

void acc0(struct param_acc0 *param, int *result) {
    *result = param->a * param->b;
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

First of all, we need to include the **locod.h** header, which contains all the declarations required to use the Locod API.

Also note that you need to add compiler directives **#ifndef LOCOD_FPGA** to limit the C file to functions to be executed in the FPGA when this define exists. The define LOCOD_FPGA is defined when the C file is passed to the tool that converts C functions to VHDL. However, it only needs the functions and not everything else, hence the addition of #ifndef to limit the C file to functions. In our example, we will add these directives around the "locod.h" include and around the main() function.

Next, calling up functions in the FPGA or CPU is done through macros in the C code to tell the Locod system where that this or that function must be executed. Apart from these macros, functions can also be called in the normal way to be executed in the CPU.

Before calling these macros to execute functions in the FPGA, the Locod system initialization function **init_locod** must be called. This function takes as arguments the number of hardware accelerators to be instantiated in the FPGA, i.e. the number of functions to be executed in the FPGA. This number can be greater than the number of functions finally instantiated, but not less, otherwise it won't work.

For our example, we'll start by defining our main function, setting the variables we'll use with our hardware accelerators. We then call the Locod initialization function with an accelerator number equal to 2:
```c
#ifndef LOCOD_FPGA
#include "locod.h"
#endif //LOCOD_FPGA

#define SIZE 20

#ifndef LOCOD_FPGA
int main(void) {
    //Variables
    struct param_acc0 param_acc_0 = { .a = 3, .b = 7};
    int result_acc_0 = 0;
    struct param_acc1 param_acc_1;
    for (int i = 0; i < SIZE; i++) {
        param_acc_1.a[i] = i;
    }
    struct result_acc1 result_acc_1 = { .a = 0, .b = 0};

    //Locod initialization
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
    - the name of the function to be executed in the FPGA
    - a pointer of param argument type for the input data 
    - a pointer of result argument type for the output data

    This macro does the same thing than launching the function the normal way in our code, but it exists to create a similarity with the FPGA macro.

    ```c
    CPU(fct, param_ptr, result_ptr)
    ```

We can then run our two example functions in the FPGA (which is what we're most interested in here). The two FPGA functions are called one after the other. The two functions will run in parallel in the FPGA:
```c
    //Launching acc1 and acc2 function in the FPGA
    FPGA(acc0, &param_acc_0, &result_acc_0, 0);
    FPGA(acc1, &param_acc_1, &result_acc_1, 1);
```

Results cannot be retrieved directly. In other words, the result pointer is not directly modified with the execution result. To retrieve the result, we need to call another function of the API, **wait_accelerator**. This function takes two arguments. The first is a pointer of result argument type (the same pointer that the one passed to the FPGA macro), and the second is the accelerator number from which we want to restrive the results (here also the same nomber than the one passed to the FPGA macro). This functions waits until the accelerator has finished, then copies the accelerator outputs to the result pointer.

Here is the code to retreve results from our two hardware accelerators example:
```c
    //Retreive outputs
    wait_accelerator(&result_acc_0, 0);
    wait_accelerator(&result_acc_1, 1);

    //Print results
    printf("Acc 0 result : %d * %d = %d\n", param_acc_0.a, param_acc_0.b, result_acc_0);
    printf("Acc 1 result : sum of input values = %f, substraction of input values = %f\n", result_acc_1.a, result_acc_1.b);
```

Finally, the locod can be de-initialized with the **deinit_locod** function, to free up memory resources taken by the system:
```c
    //Locod de-initialization
    deinit_locod();

     return 0;
} //End main()
#endif //LOCOD_FPGA
```
<br>

### Launching the Locod tool

Now that the C input code has been developed with the correct syntax for launching code in the FPGA, it needs to be passed to the Locod tool to generate :
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

The execution of the Locod tool can then be monitored with logs in the console.

Depending on the target (--target) selected, the various Dockers and tools requiered must be available on the machine. If one is missing, the script will stop.

For our example, let's say we want to test it on an Enclustra board. Assuming the dockers are present, we still need to source the Vivado environment script required for VHDL synthesis. A valid Vivado license is also required, as the nclustra embeds a Zynq Ultrascale+ XCZU6EG not available with the free version.

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