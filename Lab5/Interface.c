#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "TouchScreen.h"
#include "TFTMaster.h"
#include "ts_lcd.h"
#include "Interface.h"


stdio_init_all();
uint16_t x = 0, y = 0;
uint16_t last_x = 0, last_y = 0;
char buffer[30];
ts_lcd_init();
uint16_t buttonVal=getBotton();

void drawInterface() {
tft_drawRoundRect(10,40,72,44,8,0xFFFF);//7
tft_drawRoundRect(86,40,72,44,8,0xFFFF);//8
tft_drawRoundRect(162,40,72,44,8,0xFFFF);//9
tft_drawRoundRect(238,40,72,44,8,0xFFE0);//+ 10

tft_drawRoundRect(10,88,72,44,8,0xFFFF);//4
tft_drawRoundRect(86,88,72,44,8,0xFFFF);//5
tft_drawRoundRect(162,88,72,44,8,0xFFFF);//6
tft_drawRoundRect(238,88,72,44,8,0xFFE0);//- 11

tft_drawRoundRect(10,136,72,44,8,0xFFFF);//1
tft_drawRoundRect(86,136,72,44,8,0xFFFF);//2
tft_drawRoundRect(162,136,72,44,8,0xFFFF);//3
tft_drawRoundRect(238,136,72,44,8,0xFFE0);//x 12 

tft_drawRoundRect(10,184,72,44,8,0xFFFF);//0
tft_drawRoundRect(86,184,72,44,8,0xF800);//C 14
tft_drawRoundRect(162,184,72,44,8,0x001F);//= 15
tft_drawRoundRect(238,184,72,44,8,0xFFE0);// / 13

tft_setCursor(10, 10);
tft_setTextColor(ILI9340_WHITE);
tft_setTextSize(2);
tft_writeString(buffer);
}

uint16_t getBotton(){
    if (get_ts_lcd(&x, &y)){
        if (x<82 && x>10 && y<84 && y>40){
            return 7;
        }else if (x<158 && x>86 && y<84 && y>40){
            return 8;
        }else if (x<234 && x>162 && y<84 && y>40){
            return 9;
        }else if (x<310 && x>238 && y<84 && y>40){
            return 10;
        }else if (x<82 && x>10 && y<132 && y>88){
            return 4;
        }else if (x<158 && x>86 && y<132 && y>88){
            return 5;
        }else if (x<234 && x>162 && y<132 && y>88){
            return 6;
        }else if (x<310 && x>238 && y<132 && y>88){
            return 11;
        }else if (x<82 && x>10 && y<180 && y>136){
            return 1;
        }else if (x<158 && x>86 && y<180 && y>136){
            return 2;
        }else if (x<234 && x>162 && y<180 && y>136){
            return 3;
        }else if (x<310 && x>238 && y<180 && y>136){
            return 12;
        }else if (x<82 && x>10 && y<228 && y>184){
            return 0;
        }else if (x<158 && x>86 && y<228 && y>88){
            return 14;
        }else if (x<234 && x>162 && y<228 && y>88){
            return 15;
        }else if (x<310 && x>238 && y<228 && y>88){
            return 13;
        }
     }
}
void displayResult(){
if (buttonVal >= 0 && buttonVal <= 9) {  
        char digitStr[2]; 
        snprintf(digitStr, sizeof(digitStr), "%d", buttonVal);
        strncat(buffer, digitStr, sizeof(buffer) - strlen(buffer) - 1);
    }
}


