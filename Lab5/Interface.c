#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "TouchScreen.h"
#include "TFTMaster.h"
#include "ts_lcd.h"
#include "calculator.h"
#include "timer.h"
#include "interface.h"

void interface_init() {
    tft_begin();
    tft_setRotation(3);
    tft_fillScreen(ILI9340_BLACK);
}

void drawInterface() {
    // Row 1 buttons
    tft_drawRoundRect(10, 40, 72, 44, 8, 0xFFFF);   // 7
    tft_drawRoundRect(86, 40, 72, 44, 8, 0xFFFF);   // 8
    tft_drawRoundRect(162, 40, 72, 44, 8, 0xFFFF);  // 9
    tft_drawRoundRect(238, 40, 72, 44, 8, 0xFFE0);  // +

    // Row 2 buttons
    tft_drawRoundRect(10, 88, 72, 44, 8, 0xFFFF);   // 4
    tft_drawRoundRect(86, 88, 72, 44, 8, 0xFFFF);   // 5
    tft_drawRoundRect(162, 88, 72, 44, 8, 0xFFFF);  // 6
    tft_drawRoundRect(238, 88, 72, 44, 8, 0xFFE0);  // -

    // Row 3 buttons
    tft_drawRoundRect(10, 136, 72, 44, 8, 0xFFFF);   // 1
    tft_drawRoundRect(86, 136, 72, 44, 8, 0xFFFF);   // 2
    tft_drawRoundRect(162, 136, 72, 44, 8, 0xFFFF);  // 3
    tft_drawRoundRect(238, 136, 72, 44, 8, 0xFFE0);  // *

    // Row 4 buttons
    tft_drawRoundRect(10, 184, 72, 44, 8, 0xFFFF);   // 0
    tft_drawRoundRect(86, 184, 72, 44, 8, 0xF800);   // C
    tft_drawRoundRect(162, 184, 72, 44, 8, 0x001F);  // =
    tft_drawRoundRect(238, 184, 72, 44, 8, 0xFFE0);  // /

    tft_setTextSize(2);

    // Numbers (white)
    tft_setTextColor(ILI9340_WHITE);
    tft_setCursor(35, 55);  tft_writeString("7");
    tft_setCursor(110, 55); tft_writeString("8");
    tft_setCursor(185, 55); tft_writeString("9");

    tft_setCursor(35, 103);  tft_writeString("4");
    tft_setCursor(110, 103); tft_writeString("5");
    tft_setCursor(185, 103); tft_writeString("6");

    tft_setCursor(35, 151);  tft_writeString("1");
    tft_setCursor(110, 151); tft_writeString("2");
    tft_setCursor(185, 151); tft_writeString("3");

    tft_setCursor(35, 199);  tft_writeString("0");

    // Operators (yellow)
    tft_setTextColor(0xFFE0);
    tft_setCursor(265, 55);  tft_writeString("+");
    tft_setCursor(265, 103); tft_writeString("-");
    tft_setCursor(265, 151); tft_writeString("*");
    tft_setCursor(265, 199); tft_writeString("/");

    // Special buttons
    tft_setTextColor(0xF800);  // Red
    tft_setCursor(110, 199); tft_writeString("C");
    tft_setTextColor(0x001F);  // Blue
    tft_setCursor(185, 199); tft_writeString("=");
}

void display(const char* text, bool is_error) {
    tft_fillRect(10, 10, 300, 30, ILI9340_BLACK);
    tft_setCursor(10, 10);
    tft_setTextColor(is_error ? 0xF800 : ILI9340_WHITE);
    tft_setTextSize(2);
    tft_writeString(text);
}

uint16_t getButton(uint16_t x, uint16_t y) {
    // Row 1
    if (y >= 40 && y <= 84) {
        if (x >= 10 && x <= 82) return 7;
        if (x >= 86 && x <= 158) return 8;
        if (x >= 162 && x <= 234) return 9;
        if (x >= 238 && x <= 310) return BTN_PLUS;
    }
    // Row 2
    else if (y >= 88 && y <= 132) {
        if (x >= 10 && x <= 82) return 4;
        if (x >= 86 && x <= 158) return 5;
        if (x >= 162 && x <= 234) return 6;
        if (x >= 238 && x <= 310) return BTN_MINUS;
    }
    // Row 3
    else if (y >= 136 && y <= 180) {
        if (x >= 10 && x <= 82) return 1;
        if (x >= 86 && x <= 158) return 2;
        if (x >= 162 && x <= 234) return 3;
        if (x >= 238 && x <= 310) return BTN_MULT;
    }
    // Row 4
    else if (y >= 184 && y <= 228) {
        if (x >= 10 && x <= 82) return 0;  // Zero button
        if (x >= 86 && x <= 158) return BTN_CLEAR;
        if (x >= 162 && x <= 234) return BTN_EQUAL;
        if (x >= 238 && x <= 310) return BTN_DIV;
    }
    return BTN_NONE; 
}