#include <stdio.h>
#include "pico/stdlib.h"
#include "car_control.h"
#include "drv8871.h"
#include "hc05.h"
#include "protocol.h"
#include "sensor_fusion.h"
#include "timer.h"

// Pin definitions
#define MOTOR1_IN1 6
#define MOTOR1_IN2 7
#define MOTOR2_IN1 8
#define MOTOR2_IN2 9

int main() {
    stdio_init_all();
    
    // Initialize motors
    drv8871_dual_t motors;
    drv8871_dual_init(&motors, 
                      MOTOR1_IN1, MOTOR1_IN2,
                      MOTOR2_IN1, MOTOR2_IN2);

    // Initialize car control
    car_state_t car_state;
    car_init(&car_state, &motors);

    // Initialize HC-05 as slave
    hc05_init(HC05_SLAVE_MODE, NULL);

    // Initialize sensor fusion
    sensor_fusion_init();

    char bt_message[64];
    lsm6dsox_data_t raw_data;
    sensor_fusion_data_t fusion_data;
    
    uint32_t last_display_update = timer_read();
    uint32_t last_safety_check = timer_read();
    const uint32_t DISPLAY_UPDATE_INTERVAL = 100; // ms
    const uint32_t SAFETY_CHECK_INTERVAL = 50;    // ms

    while (1) {
        uint32_t current_time = timer_read();
        
        // Check for incoming control messages
        if (hc05_receive(bt_message, sizeof(bt_message))) {
            if (bt_message[0] == MSG_CONTROL && 
                sscanf(bt_message + 2, "%f,%f", &raw_data.acceleration_x, &raw_data.acceleration_y) == 2) {
                
                // Update sensor fusion
                if (!sensor_fusion_update(&fusion_data, &raw_data)) {
                    car_emergency_stop(&car_state);
                } else {
                    // Update car control with filtered angles
                    float tilt_angle = sensor_fusion_get_forward_tilt(&fusion_data);
                    float turn_angle = sensor_fusion_get_turn_angle(&fusion_data);
                    car_update(&car_state, tilt_angle, turn_angle);
                }

                // Only send status updates at fixed intervals
                if (timer_elapsed_ms(last_display_update, current_time) >= DISPLAY_UPDATE_INTERVAL) {
                    snprintf(bt_message, sizeof(bt_message), 
                            "%c,%d,%d\n", 
                            MSG_STATUS,
                            car_state.current_speed,
                            get_battery_level());
                    hc05_send(bt_message, strlen(bt_message));
                    last_display_update = current_time;
                }
            }
        }

        // Periodic safety checks
        if (timer_elapsed_ms(last_safety_check, current_time) >= SAFETY_CHECK_INTERVAL) {
            if (!car_check_safety(&car_state)) {
                // Send error message
                snprintf(bt_message, sizeof(bt_message), 
                        "%c,1\n", MSG_ERROR);
                hc05_send(bt_message, strlen(bt_message));
            }
            last_safety_check = current_time;
        }

        sleep_ms(1);  // Yield to avoid tight loop
    }

    return 0;
}