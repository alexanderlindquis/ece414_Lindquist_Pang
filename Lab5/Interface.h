#ifndef Interface
#define Interface

#include <stdio.h>         
#include <math.h>          
#include "pico/stdlib.h"     
#include "TFTMaster.h"   
#include "TouchScreen.h"
#include "ts_lcd.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"

uint16_t x = 0, y = 0;
uint16_t last_x = 0, last_y = 0;
char buffer[30];
ts_lcd_init();
uint16_t buttonVal;

void drawInterface();

uint16_t getButton();

void displayResult();

#endif