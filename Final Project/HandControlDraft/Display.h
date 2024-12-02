#ifndef DISPLAY_H
#define DISPLAY_H

#include "TFTMaster.h"
#include "Car_Control.h"

// Display layout constants
#define DISPLAY_WIDTH 240
#define DISPLAY_HEIGHT 320
#define SPEED_Y 50
#define DIRECTION_Y 150
#define TEXT_SIZE 3

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
    int current_speed;         // Speed as percentage
    display_state_t state;     // Current movement state
    bool emergency_stopped;    // Emergency stop status
} display_data_t;

// Function declarations
void display_init(void);
void display_update(display_data_t* data);
void display_clear_speed(void);
void display_clear_direction(void);
void display_show_emergency(void);

#endif