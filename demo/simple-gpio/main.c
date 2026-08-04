/*                      __            ___             _ 
 *                     / /    ___    / __\  ___    __| |
 *                    / /    / _ \  / /    / _ \  / _` |
 *                   / /___ | (_) |/ /___ | (_) || (_| |
 *                   \____/  \___/ \____/  \___/  \__,_|
 *
 *             ***********************************************
 *                              LoCod Project
 *                  URL: https://github.com/viveris/LoCod
 *             ***********************************************
 *                  Copyright © 2024 Viveris Technologies
 *
 *                   Developed in partnership with CNES
 *               (DTN/TVO/ET: On-Board Data Handling Office)
 *
 *   This file is part of the LoCod framework.
 *
 *   The LoCod framework is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

//This program using GPIOs is for the time only supported on PYNQ Z2 board !

#ifndef LOCOD_FPGA
#include "locod.h"
#endif //LOCOD_FPGA

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

#ifndef LOCOD_FPGA
int main(void) {
    //Variables
    struct param_acc0 param_acc_0 = {.a = 3, .b = 7};
    struct result_acc0 result_acc_0 = {.a = 0};


    init_locod(1);    
    init_gpio();

    
    //Launching acc0 function in the FPGA
    FPGA(acc0, &param_acc_0, &result_acc_0, 0);

    
    //Retreive output
    wait_accelerator(&result_acc_0, 0);

    
    //Print result
    printf("Acc 0 result : %d * %d = %d\n", param_acc_0.a, param_acc_0.b, result_acc_0.a);


    
    //Print execution time of the 1 accelerator
    printf("Acc 0 execution time = %d ns\n", get_time_ns_FPGA(0));
    gpio_pin_mode(PIN_AR0, OUTPUT);
    for(int i=0;i<5;i++){
        gpio_pin_write(PIN_AR0, HIGH);
        usleep(100000);
        gpio_pin_write(PIN_AR0, LOW);
        usleep(100000);
    }

    gpio_pin_mode(PIN_AR7, INPUT);
    gpio_pin_mode(PIN_A2, INPUT);
    gpio_pin_mode(PIN_RPIO3, INPUT);
    gpio_pin_mode(PIN_RPIO20, INPUT);

    gpio_pin_mode(PIN_AR0, OUTPUT);
    gpio_pin_mode(PIN_A0, OUTPUT);
    gpio_pin_mode(PIN_RPIO2, OUTPUT);
    gpio_pin_mode(PIN_RPIO21, OUTPUT);
    for(int i=0;i<2000;i++){
        if(gpio_pin_read(PIN_AR7) == HIGH){
            gpio_pin_write(PIN_AR0, HIGH);
        }
        else{
            gpio_pin_write(PIN_AR0, LOW);
        }

        if(gpio_pin_read(PIN_A2) == HIGH){
            gpio_pin_write(PIN_A0, HIGH);
        }
        else{
            gpio_pin_write(PIN_A0, LOW);
        }

        if(gpio_pin_read(PIN_RPIO3) == HIGH){
            gpio_pin_write(PIN_RPIO2, HIGH);
        }
        else{
            gpio_pin_write(PIN_RPIO2, LOW);
        }

        if(gpio_pin_read(PIN_RPIO20) == HIGH){
            gpio_pin_write(PIN_RPIO21, HIGH);
        }
        else{
            gpio_pin_write(PIN_RPIO21, LOW);
        }
        usleep(10000);
    }
    
    


    deinit_gpio();
    deinit_locod();

    return 0;
} //End main()
#endif //LOCOD_FPGA