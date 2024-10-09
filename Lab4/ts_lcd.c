/*
Description: Convert touchscreen coordinate to LCD coordinate 
when the pressure is detected. 
*/
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "TouchScreen.h"
#include "TFTMaster.h"
#include "ts_lcd.h"

#define tx_max 
#define tx_min 
#define ty_max 
#define ty_min 

void ts_lcd_init(){
    adc_init();
    tft_init_hw();
    tft_begin();
    tft_setRotation(3); 
    tft_fillScreen(ILI9340_BLACK);  
}
bool get_ts_lcd(uint16_t*px,uint16_t*py){
    struct TSpoint p;
    getPoint(&p);
    if (p.z){
        *px = ts_lcd_transform(p.x);
        *py = ts_lcd_transform(p.y);
        return true;
    }else 
        return false;
}
uint32_t ts_lcd_xtransform(uint32t_t tx){
    return ((tx-tx_min)*(329)/(tx_max-tx_min));
}
uint32_t ts_lcd_ytransform(uint32t_t tx){
    return ((ty-ty_min)*(239)/(ty_max-ty_min));
}