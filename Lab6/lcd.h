#ifndef lcd_h
#define lcd_h

#include <stdio.h>         
#include <math.h>         
#include "pico/stdlib.h"      
#include "TFTMaster.h"

#include "pico/stdlib.h"        
#include "TFTMaster.h"
#include "ic.h"
char buffer[10];


void lcd_init();

void dpcommand();

void dprpm();



#endif