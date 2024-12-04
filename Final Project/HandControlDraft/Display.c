#include "display.h"
#include <stdio.h>
#include <string.h>

#define TEXT_SIZE 2

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
    
    // Draw initial title
    draw_centered_text("Hand Controller", 10, ILI9340_CYAN);
}

void display_update(display_data_t* data) {
    char buf[32];
    
    // Clear previous readings (except title)
    tft_fillRect(0, 40, DISPLAY_WIDTH, DISPLAY_HEIGHT-40, ILI9340_BLACK);
    
    // Display sensor readings
    tft_setTextColor(ILI9340_WHITE);
    
    // Display pitch
    tft_setCursor(10, 50);
    snprintf(buf, sizeof(buf), "Pitch: %.1f", data->pitch);
    tft_writeString(buf);
    
    // Display roll
    tft_setCursor(10, 80);
    snprintf(buf, sizeof(buf), "Roll: %.1f", data->roll);
    tft_writeString(buf);
    
    // Display movement state
    tft_setCursor(10, 120);
    tft_setTextColor(ILI9340_YELLOW);
    tft_writeString("Movement:");
    
    tft_setCursor(10, 150);
    switch(data->movement_state) {
        case DISPLAY_FORWARD:
            tft_writeString("FORWARD");
            break;
        case DISPLAY_BACKWARD:
            tft_writeString("BACKWARD");
            break;
        case DISPLAY_TURNING_LEFT:
            tft_writeString("LEFT TURN");
            break;
        case DISPLAY_TURNING_RIGHT:
            tft_writeString("RIGHT TURN");
            break;
        default:
            tft_writeString("STOPPED");
    }
    
    // Display connection status
    tft_setCursor(10, 280);
    if (data->connected) {
        tft_setTextColor(ILI9340_GREEN);
        tft_writeString("Connected");
        
        // Show car speed if connected
        if (data->car_speed >= 0) {
            tft_setCursor(10, 200);
            snprintf(buf, sizeof(buf), "Car Speed: %d%%", data->car_speed);
            tft_writeString(buf);
        }
    } else {
        tft_setTextColor(ILI9340_RED);
        tft_writeString("No Connection");
    }
}

void display_show_connection_lost(void) {
    tft_fillRect(0, 200, DISPLAY_WIDTH, 40, ILI9340_RED);
    draw_centered_text("CONNECTION LOST", 210, ILI9340_WHITE);
}

void display_clear_screen(void) {
    tft_fillScreen(ILI9340_BLACK);
}