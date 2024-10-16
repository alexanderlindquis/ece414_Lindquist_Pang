#include <stdio.h>         
#include <math.h>          
#include "pico/stdlib.h"  
#include "pico/rand.h"    
#include "TFTMaster.h"   
#include "TouchScreen.h"
#include "ts_lcd.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "Interface.h"
enum States_t{INITIAL, INPUT1, INPUT2, ERROR, DIV0} state;
enum Operators{MULTIPLY, ADD, SUBTRACT, DIVIDE} op;
// 
int main(){
    uint16_t val1, val2;
    uint16_t x, y;
    state = INITIAL;
    while(true){
        //calculate
        if(get_ts_lcd(&x, &y)){
            buttonVal = getButton();
            if(buttonVal == 14){
                state == INITIAL;
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
                        }
                        if(buttonVal >= 10 && buttonVal 13){
                            switch(buttonVal){
                                case 10:
                                    op = ADD;
                                break;
                                case 11: 
                                    op = SUBTRACT;
                                break;
                                case 12:
                                    op = MULTIPLY;
                                break;
                                case 13: 
                                    op = DIVIDE;
                                break;
                            }
                            state = INPUT2;
                        }
                        else{
                            state = ERROR;
                        }
                    break;
                    case INPUT2:
                        if(buttonVal >= 0 && buttonVal <= 9){
                            val2 = val2 * 10 + buttonVal;
                        }
                        if(buttonVal >= 10 && buttonVal 13){
                            if(val2 == 0 && op == DIVIDE){
                                state = DIV0;
                            }
                            if(val2.isNull()){
                                state = ERROR;
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
                            }
                            switch(buttonVal){
                                case 10:
                                    op = ADD;
                                break;
                                case 11: 
                                    op = SUBTRACT;
                                break;
                                case 12:
                                    op = MULTIPLY;
                                break;
                                case 13: 
                                    op = DIVIDE;
                                break;
                            }
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
                            state = INPUT1;
                        }
                        else{
                            state = ERROR;
                        }
                    break;
                }
                //draw
                displayResult();
                drawInterface();
            }
        }
    }
}
