#ifdef BLUETHOOTH_H
#define BLUETHOOTH_H

#include "pico/stdlib.h"
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"

#define UART_ID uart0
#define BAUD_RATE 9600
#define UART_TX_PIN 0
#define UART_RX_PIN 1

char message[100];
void bluetooth_init();
void bluetooth_send(char* message);
void bluetooth_receive(char* message);


#endif // BLUETHOOTH_H