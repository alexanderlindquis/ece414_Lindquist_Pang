// display.c
#include "display.h"
#include <stdio.h>
#include <string.h>
#include "TFTMaster.h"

#define TEXT_SIZE 2
#define DISPLAY_WIDTH 240
#define DISPLAY_HEIGHT 320

static void draw_centered_text(const char* text, int y, unsigned short color) {
    int text_width = strlen(text) * 6 * TEXT_SIZE;
    int x = (DISPLAY_WIDTH - text_width) / 2;
    
    tft_setTextSize(TEXT_SIZE);
    tft_setTextColor(color);
    tft_setCursor(x, y);
    tft_writeString((char*)text);
}

void display_init(void) {
    tft_init_hw();
    tft_begin();
    tft_setRotation(0);
    tft_fillScreen(ILI9340_BLACK);
    
    // Draw title
    draw_centered_text("Hand Controller", 10, ILI9340_CYAN);
}

void display_update(const display_data_t* data) {
    char buf[32];
    
    // Clear previous readings (except title)
    tft_fillRect(0, 40, DISPLAY_WIDTH, DISPLAY_HEIGHT-40, ILI9340_BLACK);
    
    // Display angles
    tft_setTextColor(ILI9340_WHITE);
    tft_setCursor(10, 50);
    snprintf(buf, sizeof(buf), "Pitch: %.1f", data->pitch);
    tft_writeString(buf);
    
    tft_setCursor(10, 80);
    snprintf(buf, sizeof(buf), "Roll: %.1f", data->roll);
    tft_writeString(buf);
    
    // Display speed
    tft_setCursor(10, 110);
    snprintf(buf, sizeof(buf), "Speed: %d%%", data->car_speed);
    tft_writeString(buf);
    
    // Display movement state
    tft_setCursor(10, 150);
    tft_setTextColor(ILI9340_YELLOW);
    switch(data->movement_state) {
        case DISPLAY_FORWARD:
            tft_writeString("FORWARD");
            break;
        case DISPLAY_BACKWARD:
            tft_writeString("BACKWARD");
            break;
        case DISPLAY_TURNING_LEFT:
            tft_writeString("TURNING LEFT");
            break;
        case DISPLAY_TURNING_RIGHT:
            tft_writeString("TURNING RIGHT");
            break;
        default:
            tft_writeString("STOPPED");
    }
}

void display_show_error(const char* message) {
    // Draw error message in red
    tft_fillRect(0, DISPLAY_HEIGHT-60, DISPLAY_WIDTH, 40, ILI9340_RED);
    draw_centered_text(message, DISPLAY_HEIGHT-50, ILI9340_WHITE);
}