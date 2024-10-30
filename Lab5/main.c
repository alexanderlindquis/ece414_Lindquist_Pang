#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "ts_lcd.h"
#include "calculator.h"
#include "interface.h"
#include "timer.h"

#define DEBOUNCE_TIME 150

int main() {
    stdio_init_all();
    ts_lcd_init();
    interface_init();
    drawInterface();
    calculator_init();
    
    uint32_t last_touch_time = timer_read();
    bool was_touching = false;
    uint16_t x = 0, y = 0;
    uint16_t button_val = 0;
    
    // Main loop
    while(true) {
        bool touching = get_ts_lcd(&x, &y);
        uint32_t current_time = timer_read();
        
        if (touching != was_touching && timer_elapsed_ms(last_touch_time, current_time) >= DEBOUNCE_TIME) {
            if (touching) {
                button_val = getButton(x, y);
                if (button_val != BTN_NONE) {
                    tick_calculator(button_val);
                }
            }
            was_touching = touching;
            last_touch_time = current_time;
        }
        
        sleep_ms(10);
    }
}