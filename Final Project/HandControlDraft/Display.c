#include "display.h"
#include <stdio.h>

static void draw_centered_text(const char* text, int y, int size, unsigned short color) {
    int text_width = strlen(text) * 6 * size;  // 6 pixels is the base width of a character
    int x = (DISPLAY_WIDTH - text_width) / 2;
    
    tft_setTextSize(size);
    tft_setTextColor(color);
    tft_setCursor(x, y);
    tft_writeString((char*)text);
}

void display_init(void) {
    tft_init_hw();
    tft_begin();
    tft_setRotation(0);
    tft_fillScreen(ILI9340_BLACK);
    
    // Draw static labels
    draw_centered_text("Speed:", 20, 2, ILI9340_WHITE);
    draw_centered_text("Direction:", 120, 2, ILI9340_WHITE);
}

void display_clear_speed(void) {
    // Clear speed display area
    tft_fillRect(0, SPEED_Y, DISPLAY_WIDTH, 40, ILI9340_BLACK);
}

void display_clear_direction(void) {
    // Clear direction display area
    tft_fillRect(0, DIRECTION_Y, DISPLAY_WIDTH, 40, ILI9340_BLACK);
}

void display_update(display_data_t* data) {
    char buf[32];
    
    // Update speed
    display_clear_speed();
    snprintf(buf, sizeof(buf), "%d%%", data->current_speed);
    draw_centered_text(buf, SPEED_Y, TEXT_SIZE, ILI9340_GREEN);
    
    // Update direction
    display_clear_direction();
    const char* direction_text;
    unsigned short direction_color = ILI9340_CYAN;
    
    switch(data->state) {
        case DISPLAY_FORWARD:
            direction_text = "FORWARD";
            break;
        case DISPLAY_BACKWARD:
            direction_text = "BACKWARD";
            break;
        case DISPLAY_TURNING_LEFT:
            direction_text = "LEFT TURN";
            direction_color = ILI9340_YELLOW;
            break;
        case DISPLAY_TURNING_RIGHT:
            direction_text = "RIGHT TURN";
            direction_color = ILI9340_YELLOW;
            break;
        default:
            direction_text = "STOPPED";
            direction_color = ILI9340_RED;
    }
    
    draw_centered_text(direction_text, DIRECTION_Y, TEXT_SIZE, direction_color);
    
    // Show emergency stop if active
    if (data->emergency_stopped) {
        display_show_emergency();
    }
}

void display_show_emergency(void) {
    // Draw emergency stop warning
    tft_fillRect(0, 200, DISPLAY_WIDTH, 40, ILI9340_RED);
    draw_centered_text("EMERGENCY STOP", 210, 2, ILI9340_WHITE);
}