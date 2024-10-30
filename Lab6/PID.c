#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "hardware/uart.h"
#include "TouchScreen.h"
#include "TFTMaster.h"
#include "ts_lcd.h"
#include "ic.h"
#include "debounce_sw1.h"

#define UART_ID uart0
#define BAUD_RATE 115200
#define UART_TX_PIN 0
#define UART_RX_PIN 1

static enum States {}
int main(){
    ic_init();
    ts_lcd_init();
    debounce_sw1_init();

}

void interface_tick(){
    
}