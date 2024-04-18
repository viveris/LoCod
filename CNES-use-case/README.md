# CNES Use Case verification

The aim of this file is to check a demosaicing use case suitable for the LoCod project. You need to have Octave installed to be able to run the verif_use_case.sh script, to compare the outputs obtained with the LoCod execution and the Matlab execution of the use case.

Steps:

1. Compile LoCod with demo/use-case-full/main.c demonstartion file

2. Flash the card with the executable and bitstream, and launch the application by passing the imagette.bin file in input_data/ folder as an argument:
```bash
./locod-cpu -i imagette.bin
```

3. Retrieve the 2 generated files output_malvar_locod.bin and output_filter_locod.bin and place them in output_files/ folder

4. Run the verif_use_case.sh script, which will then compile the same use case with Octave, then display the Matlab and Locod results side by side for comparison