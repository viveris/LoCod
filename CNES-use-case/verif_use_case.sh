#!/bin/bash
set -e

# Compile use case demosaicing C Code and execute it
make re -C c_code
cp input_data/imagette.bin c_code/bin/
cd c_code/bin/
./use_case_ccode -i imagette.bin
mv output_malvar_ccode.bin output_filter_ccode.bin ../../output_files/
cd ../../

# Execute use case in Matlab
cp input_data/imagette.bin matlab_code/
cd matlab_code/
octave main.m
mv output_malvar_matlab.bin output_filter_matlab.bin ../output_files/
cd ../

# Display outputs of Locod, CCode and Matlab
#octave --persist display_outputs.m