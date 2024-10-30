/* 
 * File:   debounce_sw1.h
 * Author: nestorj
 *
 * Debounce a switch read by the sw_in module
 */

#ifndef DEBOUNCER1_H
#define	DEBOUNCER1_H

#include "pico/stdlib.h"

// use to schedule in main function:
#define DEBOUNCE_PD_MS 40

void debounce_sw1_init();

void debounce_sw1_tick();

// return TRUE the first time the function is called after the button has 
// been pressed.  Return FALSE until the button is released and pressed again
bool debounce_sw1_pressed();

#endif	/* DEBOUNCER_H */

