#!/bin/bash
set -e

# Execute use case in Matlab
cp input_data/imagette.bin matlab_code/
cd matlab_code/
octave main.m
mv output_malvar_matlab.bin output_filter_matlab.bin ../output_files/
cd ../

# Display outputs of Locod and Matlab
octave --persist display_outputs.m