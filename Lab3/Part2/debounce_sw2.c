/* 
 * File:   debounce_sw1.c
 * Author: nestorj
 *
 * Debounce a switch read by the sw_in module
 */

#include "stdio.h"
#include "pico/stdlib.h"
#include "debounce_sw2.h"
#include "sw_in.h"

// flag indicating button pressed and debounced
// cleared when read by debounce1_pressed()
static bool btn2_pressed;  

// state variable

static enum DB_States {NOPUSH, MAYBEPUSH, PUSHED, MAYBENOPUSH} DB_State;

void debounce_sw2_init() {
    DB_State = NOPUSH;
    btn2_pressed = false;
}

void debounce_sw2_tick() {
    bool btn2 = sw_in_read2();
    switch(DB_State) {
        case NOPUSH:
            if (btn2) DB_State = MAYBEPUSH;
            else DB_State = NOPUSH;
            break;
        case MAYBEPUSH:
            if (btn2) {
                btn2_pressed = true;
                DB_State = PUSHED;
            }
            else DB_State = NOPUSH;
            break;
        case PUSHED:
            if (btn2) DB_State = PUSHED;
            else DB_State = MAYBENOPUSH;
            break;
        case MAYBENOPUSH:
            if (btn2) DB_State = PUSHED;
            else DB_State = NOPUSH;
            break;
        default:
            DB_State = NOPUSH;
            break;
    }

    // note: no other state actions required, 
    // so we don't need a 2nd switch statement
    
}

// return TRUE the first time the function is called after the button has 
// been pressed.  Return FALSE until the button is released and pressed again
bool debounce_sw2_pressed() {
    if (btn2_pressed) {
        btn2_pressed = false; 
        return true;
    } else return false;
}



