#include "bluetooth.h"
#include "pico/stdlib.h"
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"

void bluetooth_init() {
    uart_init(UART_ID, BAUD_RATE);
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);
}
void bluetooth_sent(char* message) {
    uart_puts(UART_ID, message);
}
void bluetooth_receive(char* message) {
    uart_gets(UART_ID, message, 100);
}