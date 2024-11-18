#include <stdio.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/uart.h"
#include "controller.h"
#include "lcd.h"
#define UART_ID uart0
#define BAUD_RATE 115200
#define UART_TX_PIN 0
#define UART_RX_PIN 1

enum States {SELECT, READ} state;
enum OP {S, P, I, D} op;
float updateVal, updateDigit = 0;
bool dec; //entering decimals

int main(){
    ic_init();
    controller_init();
    lcd_init();
    state = SELECT;
    uart_init(UART_ID, BAUD_RATE);
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);
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
                    updateDigit = 0;
                    printf(".");
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
                            Kd = updateVal;
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
}
