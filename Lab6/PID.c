#include <stdio.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/uart.h"
#include "controller.h"
<<<<<<< HEAD
#include "timer.h"
#include "lcd.h"

=======
#include "lcd.h"
>>>>>>> 8a7e469995f4c22f99e1a86db6c5ac6c54816c2f
#define UART_ID uart0
#define BAUD_RATE 115200
#define UART_TX_PIN 0
#define UART_RX_PIN 1

enum States {SELECT, READ} state;
enum OP {S, P, I, D} op;
float updateVal, updateDigit = 0;
bool dec; //entering decimals

<<<<<<< HEAD
=======
int main(){
    ic_init();
    controller_init();
    state = SELECT;
    uint32_t last_time = timer_read();
    while(1){
        dprpm();
        uint32_t current_time = timer_read();
        uart_input_tick();
            if(time_elapsed_ms(last_time, current_time) >= 50){
                controller_tick();
            }
            last_time = current_time;
        
    }
}

>>>>>>> 8a7e469995f4c22f99e1a86db6c5ac6c54816c2f
void uart_input_tick(){
    if(uart_is_readable(uart0)){
        char c = uart_getc(uart0);
        sprintf(buffer,c);
        switch(state){
            case SELECT:
                if(c == 's'){
                    op = S;
                }
                else if(c == 'p'){
                    op = P;
                }
                else if(c == 'i'){
                    op = I;
                }
                else if(c == 'd'){
                    op = D;
                }
                else{
                    printf(" ?\n> bro what (-_-)\n");
                }
            break;
            case READ:
                if(c >= '0' && c <= '9'){
                    if(dec){
                        updateVal += (float)(c - '0') / pow(10, updateDigit + 1);
                    }
                    else{
                        updateVal = updateVal * 10 + (float)(c - '0');
                    }
                }
                else if(c == '.'){
                    dec = true;
                }
                else if(c == '\r'){
                    switch(op){
                        case S:
                            desired = updateVal;
                        break;
                        case P:
                            Kp = updateVal;
                        break;
                        case I:
                            Ki = updateVal;
                        break;
                        case D:
                            Kp = updateVal;
                        break;
                    }
                    dec = false;
                    updateVal = updateDigit = 0;
                    state = SELECT;
                    printf("%f",updateVal);
                }
                else{
                    printf(" ?\n> bruh (O_O)\n");
                    state = SELECT;
                }
            break;
        }
    }
<<<<<<< HEAD
}
int main(){
    ic_init();
    controller_init();
    lcd_init();
    state = SELECT;
    uint32_t last_time = timer_read();
    while(1){
        dpcommand();
        dprpm();
        uint32_t current_time = timer_read();
        uart_input_tick();
            if(timer_elapsed_ms(last_time, current_time) >= 50){
                controller_tick();
            }
            last_time = current_time;
    }
}


=======
}
>>>>>>> 8a7e469995f4c22f99e1a86db6c5ac6c54816c2f
