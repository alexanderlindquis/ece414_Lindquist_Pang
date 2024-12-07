#include "hc05.h"
#include <string.h>
#include <stdio.h>

#define UART_ID uart0
#define BAUD_RATE 9600
#define UART_TX_PIN 0
#define UART_RX_PIN 1
#define STATE_PIN 2

static bool is_initialized = false;

bool hc05_init(bool is_master, const char* peer_address) {
    // Initialize UART
    uart_init(UART_ID, BAUD_RATE);
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);
    
    // Initialize state pin
    gpio_init(STATE_PIN);
    gpio_set_dir(STATE_PIN, GPIO_OUT);
    gpio_put(STATE_PIN, 0);  // Normal mode
    
    is_initialized = true;
    return true;
}

bool hc05_send(const char* message, size_t length) {
    if (!is_initialized || !uart_is_writable(UART_ID)) {
        return false;
    }
    
    uart_write_blocking(UART_ID, (const uint8_t*)message, length);
    return true;
}

bool hc05_receive(char* message, size_t max_length) {
    if (!is_initialized || !uart_is_readable(UART_ID)) {
        return false;
    }
    
    size_t count = 0;
    uint32_t start = to_ms_since_boot(get_absolute_time());
    
    while (count < max_length - 1) {
        if (uart_is_readable(UART_ID)) {
            char c = uart_getc(UART_ID);
            if (c == '\n') {
                message[count] = '\0';
                return true;
            }
            message[count++] = c;
        }
        
        // 100ms timeout
        if (to_ms_since_boot(get_absolute_time()) - start > 100) {
            message[count] = '\0';
            return count > 0;
        }
    }
    
    message[max_length - 1] = '\0';
    return true;
}

bool hc05_is_connected(void) {
    return is_initialized && uart_is_enabled(UART_ID);
}