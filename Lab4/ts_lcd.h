/*
Header file for converting coordinate of touchscreen to 
LCD coordinate
*/
#ifndef TS_LCD_H
#define TS_LCD_H

#include "TFTMaster.h"
#include "TouchScreen.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"


bool get_ts_lcd(uint16_t *px, uint16_t *py);

void ts_lcd_init();

#endif