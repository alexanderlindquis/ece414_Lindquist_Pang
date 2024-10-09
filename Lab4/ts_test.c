/*
Description: Draw crosshair and show the coordinates on LCD 
based on where your finger pressed

*/
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "TouchScreen.h"
#include "TFTMaster.h"
#include "ts_lcd.h"


void crosshair(uint16_t x, uint16_t y){//function for drawing a 10 by 10 crosshair
tft_drawLine(x-5,y,x+5,y,0x07FF);
tft_drawLine(x,y-5,x,y+5,0x07FF);
}

int main(){
    char buffer[30];
    uint16_t x=0;
    uint16_t y=0;
    ts_lcd_init();
    while(true){
        if (get_ts_lcd(&x,&y)){
        crosshair(x,y);
        tft_setCursor(20, 100);
        tft_setTextColor(ILI9340_WHITE);
        sprintf(buffer,"x: %d, y: %d, z: %d", x, y, p.z);
        tft_writeString(buffer);

        sleep_ms(100);
        }

    }
}