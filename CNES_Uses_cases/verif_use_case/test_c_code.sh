#                     __            ___             _ 
#                    / /    ___    / __\  ___    __| |
#                   / /    / _ \  / /    / _ \  / _` |
#                  / /___ | (_) |/ /___ | (_) || (_| |
#                  \____/  \___/ \____/  \___/  \__,_|
#
#            ***********************************************
#                             LoCod Project
#                 URL: https://github.com/viveris/LoCod
#            ***********************************************
#                 Copyright © 2024 Viveris Technologies
#
#                  Developed in partnership with CNES
#              (DTN/TVO/ET: On-Board Data Handling Office)
#
#  This file is part of the LoCod framework.
#
#  The LoCod framework is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 3 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

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