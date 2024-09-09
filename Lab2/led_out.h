#ifndef LED_OUT_H
#define LED_OUT_H

#include "pico/stdlib.h"

void led_out_init();

void led_out_write(uint8_t val);

#endif