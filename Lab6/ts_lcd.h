// ts_lcd.h
#ifndef TS_LCD_H
#define TS_LCD_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "TouchScreen.h"
#include "TFTMaster.h"

void ts_lcd_init();

bool get_ts_lcd(uint16_t *px, uint16_t *py);

uint16_t ts_lcd_xtransform(uint16_t tx);

uint16_t ts_lcd_ytransform(uint16_t ty);

#endif 