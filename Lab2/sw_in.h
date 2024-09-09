#ifndef SW_IN_H
#define SW_IN_H

#include "pico/stdlib.h"

#define SW1_PIN 16
#define SW2_PIN 17

void sw_in_init();

bool sw_in_read1();

bool sw_in_read2();

#endif
