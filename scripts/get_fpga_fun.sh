#!/bin/bash
grep -oP '(?<=FPGA\().*?(?=,)' $1
