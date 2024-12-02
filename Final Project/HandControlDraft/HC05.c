#include "hc05.h"
#include <string.h>
#include <stdio.h>

static hc05_status_t hc05_status = {0};

static bool hc05_send_command(const char* command) {
    if (!uart_is_writable(HC05_UART_ID)) {
        return false;
    }
    
    uart_puts(HC05_UART_ID, command);
    uart_puts(HC05_UART_ID, "\r\n");
    
    // Wait for response
    char response[64] = {0};
    uint32_t start = to_ms_since_boot(get_absolute_time());
    
    while (to_ms_since_boot(get_absolute_time()) - start < 1000) {
        if (uart_is_readable(HC05_UART_ID)) {
            uart_read_blocking(HC05_UART_ID, (uint8_t*)response, 1);
            if (strstr(response, "OK")) {
                return true;
            }
        }
    }
    return false;
}

void hc05_init(bool is_master, const char* peer_address) {
    // Initialize GPIO
    gpio_init(HC05_STATE_PIN);
    gpio_set_dir(HC05_STATE_PIN, GPIO_OUT);
    
    // Initialize UART
    uart_init(HC05_UART_ID, HC05_BAUD_RATE);
    gpio_set_function(HC05_UART_TX, GPIO_FUNC_UART);
    gpio_set_function(HC05_UART_RX, GPIO_FUNC_UART);
    
    // Enter AT mode
    gpio_put(HC05_STATE_PIN, 1);
    sleep_ms(100);
    
    // Configure module
    char cmd[64];
    snprintf(cmd, sizeof(cmd), "AT+ROLE=%d", is_master ? 1 : 0);
    hc05_send_command(cmd);
    
    if (is_master && peer_address) {
        snprintf(cmd, sizeof(cmd), "AT+BIND=%s", peer_address);
        hc05_send_command(cmd);
        hc05_send_command("AT+CMODE=0");
    }
    
    // Exit AT mode
    gpio_put(HC05_STATE_PIN, 0);
    sleep_ms(100);
    
    hc05_status.state = HC05_STATE_DISCONNECTED;
}

bool hc05_send(const char* message, size_t length) {
    if (!uart_is_writable(HC05_UART_ID)) {
        return false;
    }
    
    uart_write_blocking(HC05_UART_ID, (const uint8_t*)message, length);
    hc05_status.last_message_time = to_ms_since_boot(get_absolute_time());
    return true;
}

bool hc05_receive(char* message, size_t max_length) {
    size_t count = 0;
    uint32_t start = to_ms_since_boot(get_absolute_time());
    
    while (count < max_length - 1) {
        if (uart_is_readable(HC05_UART_ID)) {
            message[count] = uart_getc(HC05_UART_ID);
            if (message[count] == '\n') {
                message[count] = '\0';
                return true;
            }
            count++;
        }
        
        if (to_ms_since_boot(get_absolute_time()) - start > 100) {
            message[count] = '\0';
            return count > 0;
        }
    }
    message[max_length - 1] = '\0';
    return true;
}