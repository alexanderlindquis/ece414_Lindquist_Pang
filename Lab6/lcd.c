#include <stdio.h>         
#include <math.h>         
#include "pico/stdlib.h"   
#include "pico/rand.h"     
#include "TFTMaster.h"
#include "lcd.h"
#include "timer.h"
#include "controller.h"

uint32_t time_start, time_b;

void lcd_init(){
    stdio_init_all();
    tft_init_hw();
    tft_begin();
    tft_setRotation(3);
    tft_fillScreen(ILI9340_BLACK);
    tft_setTextColor(ILI9340_BLUE);
    tft_setTextSize(2);
    time_start = timer_read();
}
void dpcommand(){
    tft_setCursor(0,0);
    tft_setTextColor(ILI9340_WHITE);
    tft_writeString(buffer);
}
void dprpm(){
    uint32_t rpm = ic_getrpm();
    tft_setCursor(300,0);
    tft_setTextColor(ILI9340_WHITE);
    sprintf(buffer, "RPM =%u ",rpm);
    tft_writeString(buffer);
    if(!NO_CAPTURE_READY){
        time_b = timer_read();
        uint32_t elapsed = (timer_elapsed_ms(time_start,time_b));
        uint16_t y = (3000-rpm)/20; 
        uint16_t x = elapsed/10;
        
        tft_drawPixel(x,y,ILI9340_RED);
        tft_drawPixel(x,desired,ILI9340_GREEN);
        tft_drawPixel(x,pwm_level,ILI9340_BLUE);
        if(x>=300){
            tft_fillScreen(ILI9340_BLACK);
            time_start = timer_read;
        }
        if (y < 0) y = 0;
        if (y >= 240) y = 239;
    }
   }
