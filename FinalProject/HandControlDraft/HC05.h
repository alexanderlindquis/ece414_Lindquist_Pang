#ifndef HC05_H
#define HC05_H

#include "pico/stdlib.h"
#include "hardware/uart.h"

#define HC05_UART_ID uart0
#define HC05_BAUD_RATE 9600
#define HC05_UART_TX 0
#define HC05_UART_RX 1
#define HC05_STATE_PIN 2

#define HC05_MASTER_MODE 1
#define HC05_SLAVE_MODE 0

typedef enum {
    HC05_STATE_DISCONNECTED,
    HC05_STATE_CONNECTED,
    HC05_STATE_ERROR
} hc05_connection_state_t;

typedef struct {
    hc05_connection_state_t state;
    uint32_t last_message_time;
    char error_message[50];
} hc05_status_t;

void hc05_init(bool is_master, const char* peer_address);
bool hc05_send(const char* message, size_t length);
bool hc05_receive(char* message, size_t max_length);
bool hc05_check_connection(void);
hc05_status_t hc05_get_status(void);
void hc05_reset(void);

#endif // HC05_H
