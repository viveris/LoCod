#!/bin/bash

make re -C c_code

rm -rf verification_data/result_ccode_*
c_code/bin/demosaicing verification_data/imagette.bin 1024 1024 verification_data/result_ccode_r.bin verification_data/result_ccode_g.bin verification_data/result_ccode_b.bin

r_errors=$(cmp -l verification_data/result_ccode_r.bin verification_data/result_matlab_r.bin | wc -l)
g_errors=$(cmp -l verification_data/result_ccode_g.bin verification_data/result_matlab_g.bin | wc -l)
b_errors=$(cmp -l verification_data/result_ccode_b.bin verification_data/result_matlab_b.bin | wc -l)

r_percent_error=$(bc -l <<< "($r_errors*100)/(1024*1024)")
g_percent_error=$(bc -l <<< "($g_errors*100)/(1024*1024)")
b_percent_error=$(bc -l <<< "($b_errors*100)/(1024*1024)")

echo "Percentage of red errors : $r_percent_error %"
echo "Percentage of green errors : $g_percent_error %"
echo "Percentage of blue errors : $b_percent_error %"