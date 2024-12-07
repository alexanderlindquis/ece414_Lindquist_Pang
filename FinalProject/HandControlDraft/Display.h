#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdbool.h>

typedef enum {
    DISPLAY_STOPPED,
    DISPLAY_FORWARD,
    DISPLAY_BACKWARD,
    DISPLAY_TURNING_LEFT,
    DISPLAY_TURNING_RIGHT
} display_state_t;

typedef struct {
    float pitch;
    float roll;
    int car_speed;
    display_state_t state;
    bool connected;
} display_data_t;

void display_init(void);
void display_update(const display_data_t* data);
void display_show_error(const char* message);

#endif
