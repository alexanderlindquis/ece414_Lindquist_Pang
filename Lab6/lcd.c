#include <stdio.h>         
#include <math.h>         
#include "pico/stdlib.h"   
#include "pico/rand.h"     
#include "TFTMaster.h"
#include "lcd.h"
#include "timer.h"
#include "controller.h"

#define DISPLAY_WIDTH 320
#define DISPLAY_HEIGHT 240
#define PLOT_MARGIN 20
#define RPM_SCALE_FACTOR 20  
#define TIME_SCALE_FACTOR 10 

char buffer[32];
uint32_t time_start;
static uint16_t plot_x = 0;

void lcd_init() {
    stdio_init_all();
    tft_init_hw();
    tft_begin();
    tft_setRotation(3);
    lcd_clear_screen();
    draw_grid();
    time_start = timer_read();
}

void lcd_clear_screen() {
    tft_fillScreen(ILI9340_BLACK);
    plot_x = 0;
}


void dpcommand() {
    // Clear the command area
    tft_fillRect(0, 0, 150, 20, ILI9340_BLACK);
    tft_setCursor(0, 0);
    tft_setTextColor(ILI9340_WHITE);
    tft_setTextSize(2);
    tft_writeString(buffer);
}

void dprpm() {
    uint32_t rpm = ic_getrpm();
    
    // Update RPM display
    tft_fillRect(200, 0, 120, 20, ILI9340_BLACK);
    tft_setCursor(200, 0);
    tft_setTextColor(ILI9340_WHITE);
    tft_setTextSize(2);
    sprintf(buffer, "RPM:%u", rpm);
    tft_writeString(buffer);
    
    if (rpm != NO_CAPTURE_READY) {
        uint32_t current_time = timer_read();
        uint32_t elapsed = timer_elapsed_ms(time_start, current_time);
        
        // Calculate plot coordinates
        uint16_t x = PLOT_MARGIN + plot_x;
        uint16_t y_rpm = DISPLAY_HEIGHT - PLOT_MARGIN - (rpm / RPM_SCALE_FACTOR);
        uint16_t y_desired = DISPLAY_HEIGHT - PLOT_MARGIN - (desired / RPM_SCALE_FACTOR);
        uint16_t y_pwm = DISPLAY_HEIGHT - PLOT_MARGIN - ((pwm_level * 3000 / 0xFFFF) / RPM_SCALE_FACTOR);
        
        // Constrain y values
        y_rpm = (y_rpm < PLOT_MARGIN) ? PLOT_MARGIN : 
                (y_rpm > DISPLAY_HEIGHT - PLOT_MARGIN) ? DISPLAY_HEIGHT - PLOT_MARGIN : y_rpm;
        y_desired = (y_desired < PLOT_MARGIN) ? PLOT_MARGIN : 
                    (y_desired > DISPLAY_HEIGHT - PLOT_MARGIN) ? DISPLAY_HEIGHT - PLOT_MARGIN : y_desired;
        y_pwm = (y_pwm < PLOT_MARGIN) ? PLOT_MARGIN : 
                (y_pwm > DISPLAY_HEIGHT - PLOT_MARGIN) ? DISPLAY_HEIGHT - PLOT_MARGIN : y_pwm;
        
        // Plot points
        tft_drawPixel(x, y_rpm, ILI9340_RED);      // Actual RPM
        tft_drawPixel(x, y_desired, ILI9340_GREEN); // Desired RPM
        tft_drawPixel(x, y_pwm, ILI9340_BLUE);     // PWM level
        
        // Increment plot position
        plot_x++;
        
        // Reset display if we reach the end
        if (x >= DISPLAY_WIDTH - PLOT_MARGIN) {
            lcd_clear_screen();
            draw_grid();
            time_start = timer_read();
            plot_x = 0;
        }
    }
}