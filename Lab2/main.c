#include "pico/stdlib.h"
#include "sw_in.h"
#include "led_out.h"

main(){
    bool in1, in2;
    uint16_t out;
    sw_in_init();
    led_out_init();
    while(1) {
        in1 = sw_in_read1();
        in2 = sw_in_read2();
        //Compute 'out'
        if (in1 && in2){
            out = 0x00ff;
        }else if (in1 && !in2){
            out = 0x0001;
        }else if (!in && in2){
            out = 0x0080;
        }else if (!in && !in2){
            out = 0x0000;
        }

        led_out_write(out);
    }
}