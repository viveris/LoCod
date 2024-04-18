# Steps to add a new target

In this document, we present the steps involved in adding a target to the LoCod project. 

Various parts of the project are then modified. We will separate the necessary steps into two distinct parts: the changes to be made to compile the CPU code and adapt the FPGA design to the new target.

The aim will be to make these modifications while preserving the overall architecture of the various parts concerned, so as to end up with a homogeneous repository in terms of architecture.

<br>

## FPGA code adaptation

First of all, the FPGA design must be modified to suit the new target. For simplicity's sake, we will limit ourselves to a Xilinx or NanoXplore target, but the process of adding a new target should be broadly similar for targets from other manufacturers.

The principle of LoCod generation for the CPU part is to first generate a pure RTL design called locod_top, which contains all the LoCod architecture with control registers, accelerators generated from C code, interconnects, etc., and then to include this locod_top in a target-specific global top level. It is in this latter top level that the locod_top will be connected to the SoC via AXI interfaces.

### 1. Add an HDL top level for the new target

The first step in adapting the FPGA design is to create a top-level component containing the locod_top and all the other components needed to connect the AXI interfaces of the locod_top to the rest of the SoC. In most cases, it's just a matter of adding an SoC wrapper that defines all access interfaces to the rest of the SoC, and then connecting the locod_top to this SoC wrapper. When instantiating the locod_top, a number of general parameters need to be modified to adapt the AXI bus size to the new target. In the case of Xilinx targets, the SoC wrapper is added via a design block created by a TCL script.

All target-specific sources are then added to the **submodules/locod-fpga/manufacturer/new_target/src,scripts,...** folder.

It is also in this step that, depending on the target, we will define the base address of the LoCod control registers. This can be changed using the generic BASE_ADDRESS parameter of the locod_top component.

### 2. Add new synthesis environment

If the newly added target is neither a Xilinx target nor a NanoXplore target, a new environment must be created to execute the FPGA design synthesis.

Wherever possible, the tools needed for synthesis should be included in a docker to make it easier to port LoCod to new machines. If this is not possible or too complex, then the tools required for synthesis should be added as a dependency of the LoCod, as is the case with Vivado, for example.

### 3. Modify FPGA generation scripts

Then, we need to modify the scripts that will synthesize the complete fpga design, including the LoCod, to generate the bitstream for the new target.

Depending on the target, the scipts can be of different kinds. For example, Xilinx uses TCL scripts, while NanoXplore uses Python scripts. For each manufacturer, there's a global generation script that adds the common LoCod sources, i.e. the locod_top component. This script then calls one or more generation scripts specific to each target, and it is these scripts that add the specific sources added above. The architecture is then as follows:

```
submodules/locod-fpga/
    |_manufacturer/
        |_new_target/
            |_src/
                |_...
            |_scripts/
                |_...
            configure_project_new_target.xxx
        generate_manufacturer_project.xxx
```

We therefore need to modify the generate_manufacturer_project.xxx script so that it calls scripts specific to the new target. When this script is called by locod.sh (general LoCod script), a parameter is passed to specify the target chosen by the user. This parameter must then be used to call this or that sub-script to configure the project. For example, for Xilinx targets we have:
```tcl
# Target specific operations
switch $target {
    ultra96 {
        source ultra96/configure_project_ultra96.tcl
    }
    enclustra {
        source enclustra/configure_project_enclustra.tcl
    } 
    default {
        puts "$target not available"
        break
    }
}
```

<br>

## CPU code adaptation

Next, we need to modify the CPU code compilation. Depending on the target, the CPU architecture may vary, as may the various libraries and drivers required to run code on that target.

The principle of CPU compilation for the LoCod project is to copy the C input file into the locod-core/src folder to form a classic C Makefile compilation project, then launch a docker containing the compilation environment for the chosen target by linking the locod-core folder to then perform compilation in the docker.

### 4. Adding new SDK

The first step in adding a target is therefore to provide a new SDK containing the compilation environment for the new target. This SDK must be docked so that it can be easily installed and used on a new machine.

This docker must then be ready to receive a folder containing a Makefile project and compile it. Let's say, for example, that the docker is then named **sdk-new-target**, and that we mount the Makefile project in **/workdir** in the docker to compile it there.

### 5. Modify compilation Makefile

Once the new SDK is ready for use, modifications may also be required to the Makefile compilation, including the addition of libraries, link files or compilation options specific to the new target.

If we look at the Makefile present in locod-code, we can observe conditions on the chosen target. The target name used for these conditions is the one that will be passed as a parameter to the LoCod script with the -t option.

If you have specific compiler options to be used only for the new target, then you need to modify the Makefile by adding the modifications encapsulated in:
```Makefile
ifeq ($(TARGET), new_target)
    ...
endif
```

### 6. Addapt the LoCod C code

Inside the C code, a specific header for each target defines various constants for that target, such as the address where the DMA zone is to be accessed, the address where the control registers are to be read/written, the FPGA clock frequency, etc... The base address for the Lood control registers must correspond to the one configured in the [step n°1](#1-add-an-hdl-top-level-for-the-new-target). This file also defines whether the CPu code will be compiled for baremetal or Linux.

We therefore need to create a new definition file named, for example, **new_target_defs.h**, which we place in the locod-core/src/ folder. This file will then be included or not in the locod.h header according to a define passed at compile time in the Makefile:

locod.h:
```c
#elif defined(TARGET_new_target)
    #include "new_target_defs.h"
```

Makefile:
```Makefile
ifeq ($(TARGET), new_target)
    CFLAGS += -DTARGET_${TARGET}
    ...
endif
```

<br>

## Final changes

### 7. Modify the general LoCod script

The final step in adding a new target is to modify the global LoCod script, **locod.sh**, to call the new files created or modified previously.

Targets are managed in the locod.sh script using an argument passed by the user via the -t (or --target) option when the script is called. The first step is then to modify the argument handling to accept the new target. It is also advisable to modify the LoCod script's help to inform the user of the possibility of using the new target.

Once the new target option has been defined in the script, say for example as we've called it throughout this README **new_target**, we can then rely on it to perform the necessary operations.

First of all, we need to check all the tools required to use this target in the dependency checks section of the locod.sh script.

We then need to change the compilation part for the CPU. Based on the new_target condition, call the docker SDK for this new target, pass the locod-core to the docker, compile in it and save the executable created.

The same goes for FPGA synthesis. Depending on the new_target condition, you need to launch the synthesis tool in a docker or not, then call the script corresponding to the manufacturer, passing the new_target target as a parameter so that it calls the specific script for this target and configures the FPGA project correctly.

Once the locod.sh script has been modified, you can launch the LoCod tool by selecting the target new_target, and test the compilation of the various available demos.

<br>



