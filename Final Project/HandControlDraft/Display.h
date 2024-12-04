#ifndef DISPLAY_H
#define DISPLAY_H

#include "TFTMaster.h"
#include <stdbool.h>

// Display layout constants
#define DISPLAY_WIDTH 240
#define DISPLAY_HEIGHT 320

// Movement states for display
typedef enum {
    DISPLAY_STOPPED,
    DISPLAY_FORWARD,
    DISPLAY_BACKWARD,
    DISPLAY_TURNING_LEFT,
    DISPLAY_TURNING_RIGHT
} display_state_t;

// Display data structure
typedef struct {
    float pitch;
    float roll;
    int car_speed;
    display_state_t movement_state;
    bool connected;
} display_data_t;

// Function declarations
void display_init(void);
void display_update(display_data_t* data);
void display_show_connection_lost(void);
void display_clear_screen(void);

#endif // DISPLAY_H