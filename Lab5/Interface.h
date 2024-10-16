#ifndef Interface
#define Interface

#include <stdio.h>         
#include <math.h>          
#include "pico/stdlib.h"  
#include "pico/rand.h"    
#include "TFTMaster.h"   
#include "TouchScreen.h"
#include "ts_lcd.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"

void drawInterface();

uint16_t getBotton();

void displayResult();

#endif