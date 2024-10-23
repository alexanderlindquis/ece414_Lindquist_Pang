#include <stdio.h>         
#include <math.h>          
#include "pico/stdlib.h"   
#include "TFTMaster.h"   
#include "TouchScreen.h"
#include "ts_lcd.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "timer.h"
#include "debounce_sw1.h"
#include "Interface.h"
uint16_t val1, val2;
uint16_t x, y;
uint32_t t1, t2;

enum States_t{INITIAL, INPUT1, INPUT2, ERROR, DIV0} state;
enum Operators{MULTIPLY, ADD, SUBTRACT, DIVIDE} op;
// 
void calculator_tick(){
    char digitStr;
     
            buttonVal = getButton();
            if(buttonVal == 14){
                state == INITIAL;
                val1 = val2 = 0;
                snprintf(buffer, sizeof(buffer), "%d", 0);
            }
            else{
                switch(state){
                    case INITIAL:
                        if(buttonVal >= 0 && buttonVal <= 9){
                            state = INPUT1;
                        }
                        else{
                            state = ERROR;
                        }
                    break;
                    case INPUT1:
                    //Button val added to buffer string / val1
                        if(buttonVal >= 0 && buttonVal <= 9){
                            val1 = val1 * 10 + buttonVal;
                            displayResult();
                        }
                        if(buttonVal >= 10 && buttonVal <= 13){
                            switch(buttonVal){
                                case 10:
                                    op = ADD;
                                    digitStr = "+";
                                break;
                                case 11: 
                                    op = SUBTRACT;
                                    digitStr = "-";
                                break;
                                case 12:
                                    op = MULTIPLY;
                                    digitStr = "*";
                                break;
                                case 13: 
                                    op = DIVIDE;
                                    digitStr = "/";
                                break;
                            }
                            strncat(buffer, digitStr, sizeof(buffer) - strlen(buffer) - 1);
                            state = INPUT2;
                        }
                        else{
                            state = ERROR;
                        }
                    break;
                    case INPUT2:
                        if(buttonVal >= 0 && buttonVal <= 9){
                            val2 = val2 * 10 + buttonVal;
                            displayResult();
                        }
                        if(buttonVal >= 10 && buttonVal<= 13){
                            if(val2 == 0 && op == DIVIDE){
                                state = DIV0;
                            }
                            else{
                                switch(op){
                                    case ADD:
                                        val1 = val1 + val2;
                                    break;
                                    case SUBTRACT:
                                        val1 = val1 - val2;
                                    break;
                                    case MULTIPLY:
                                        val1 = val1 * val2;
                                    break;
                                    case DIVIDE:
                                        val1 = val1 / val2;
                                }
                                snprintf(buffer, sizeof(buffer), "%d", val1);
                            }
                            switch(buttonVal){
                                case 10:
                                    op = ADD;
                                    digitStr = "+";
                                break;
                                case 11: 
                                    op = SUBTRACT;
                                    digitStr = "-";
                                break;
                                case 12:
                                    op = MULTIPLY;
                                    digitStr = "*";
                                break;
                                case 13: 
                                    op = DIVIDE;
                                    digitStr = "/";
                                break;
                            }
                            strncat(buffer, digitStr, sizeof(buffer) - strlen(buffer) - 1);
                        }
                        if(buttonVal == 15){
                            switch(op){
                                    case ADD:
                                        val1 = val1 + val2;
                                    break;
                                    case SUBTRACT:
                                        val1 = val1 - val2;
                                    break;
                                    case MULTIPLY:
                                        val1 = val1 * val2;
                                    break;
                                    case DIVIDE:
                                        val1 = val1 / val2;
                            }
                            snprintf(buffer, sizeof(buffer), "%d", val1);
                            state = INPUT1;
                        }
                        else{
                            state = ERROR;
                        }
                    break;
                    case DIV0:
                        snprintf(buffer, sizeof(buffer), "DIV0");
                        sleep_ms(1000);
                        state = INITIAL;
                    break;
                    case ERROR:
                        snprintf(buffer, sizeof(buffer), "ERR");
                        sleep_ms(1000);
                        state = INITIAL;
                    break;
                }
            
     }
}
int main(){
    ts_lcd_init()
    state = INITIAL;
    t1 = timer_read();
    while(true){
       t2 = timer_read();
       if(timer_elapse_ms(t1, t2)>= DEBOUNCE_PD_MS){
            debounce_sw1_tick();
            t1 = t2;
       }
       if(debounce_sw1_pressed()){
        calculator_tick();
       }
    
       drawInterface();
    }
}
