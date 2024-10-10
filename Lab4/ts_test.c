/*
Draw crosshair and show the coordinates on LCD
based on where your finger pressed

*/
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "TouchScreen.h"
#include "TFTMaster.h"
#include "ts_lcd.h"

void crosshair(uint16_t x, uint16_t y)
{
    tft_drawLine(x - 5, y, x + 5, y, 0x07FF);
    tft_drawLine(x, y - 5, x, y + 5, 0x07FF);
}

int main()
{
    stdio_init_all();
    uint16_t x = 0, y = 0;
    uint16_t last_x = 0, last_y = 0;
    char buffer[30];
    bool touch_detected = false;

    ts_lcd_init();

    while (true)
    {
        if (get_ts_lcd(&x, &y))
        {
            tft_setTextColor(ILI9340_WHITE);
            tft_setTextSize(2);
           
           // Update last touch coordinates
            last_x = x;
            last_y = y; 

            // Clear the screen only when a new touch is detected
            tft_fillScreen(ILI9340_BLACK); 

            crosshair(x, y);
            tft_setCursor(20, 100);
            tft_setTextColor(ILI9340_WHITE);
            sprintf(buffer, "x: %d, y: %d", x, y);
            tft_writeString(buffer);

            sleep_ms(100); 
        }
        else 
        {
            // Display the last coordinates and crosshair when no touch is detected
            crosshair(last_x, last_y); 
            tft_setCursor(20, 100);
            tft_setTextColor(ILI9340_WHITE);
            sprintf(buffer, "x: %d, y: %d", last_x, last_y);
            tft_writeString(buffer);
        }
    }
}