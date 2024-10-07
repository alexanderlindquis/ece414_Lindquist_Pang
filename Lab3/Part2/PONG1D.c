// Main module for PONG1D Game Logic

//Include necessary modules/libraries
#include "sw_in.h"
#include "debounce_sw1.h"
#include "debounce_sw2.h"
#include "led_out.h"
#include "timer.h"
#define LED_PIN 2
#define LED_PIN2 10
#define PONGPD_MS 300

static enum PONG_States {LEFT, RIGHT, SERVE, BLINKL,BLINKR} PONG_State;
const uint32_t MASK_9_2 = 0x000003fc;
uint32_t outval = 0x80;
bool P0 = false;
//main logic
int main(){
  gpio_init(LED_PIN);
  gpio_set_dir(LED_PIN, GPIO_OUT);
  sw_in_init();
  debounce_sw1_init();
  debounce_sw2_init();
  uint32_t tBtn1, tCurrent1,tBtn2, tCurrent2,tPong,tPong2;
  PONG_State = SERVE;
  tBtn1 = timer_read();
  tBtn2 = timer_read();
  tPong = timer_read();
  
  gpio_init_mask(MASK_9_2);
  gpio_set_dir_out_masked(MASK_9_2);
  while(true){
    gpio_put_masked(MASK_9_2, outval << 2);
    tCurrent1 = timer_read();
    tCurrent2 = timer_read();
    tPong2 = timer_read();
    if(timer_elapsed_ms(tBtn1, tCurrent1) >= DEBOUNCE_PD_MS){
      debounce_sw1_tick();
      tBtn1=tCurrent1;
      
    }
    if(timer_elapsed_ms(tBtn2, tCurrent2) >= DEBOUNCE_PD_MS){
      debounce_sw2_tick();
      tBtn2=tCurrent2;
      
    }
    if(timer_elapsed_ms(tPong, tPong2) >= PONGPD_MS){
      pong_tick();
      tPong = tPong2;
    }
  }
}

//Update Game State based on button presses, current active player, 
void pong_tick(){
  switch(PONG_State){
    case SERVE:
      if(P0){
        outval = 0x80;
      }else 
        outval = 0x01;
      if(P0 && debounce_sw1_pressed()){
        PONG_State = LEFT;
      }
      else if(!P0 && debounce_sw2_pressed()){
        PONG_State = RIGHT;
      }
      break;

    case LEFT:
      outval = outval >> 1;
      if (outval == 0x01) {
        if (debounce_sw2_pressed()) {
          PONG_State = RIGHT;
        } else {
          PONG_State = BLINKL;
        }
      } else if (debounce_sw2_pressed()) {
        PONG_State = BLINKL;
      }
      break;
    case RIGHT:
        outval = outval << 1;
        if (outval == 0x80) {
          if (debounce_sw1_pressed()) {
            PONG_State = LEFT;
          } else {
            PONG_State = BLINKR;
          }
        } else if (debounce_sw1_pressed()) {
          PONG_State = BLINKR;
        }
        break;

    case BLINKL:
        gpio_put(LED_PIN2, 1);
        sleep_ms(100);
        gpio_put(LED_PIN2, 0);
        sleep_ms(100);
        gpio_put(LED_PIN2, 1);
        sleep_ms(100);
        gpio_put(LED_PIN2, 0);
        sleep_ms(100);
        P0 = true;
        PONG_State = SERVE;
      break;

    case BLINKR:
        gpio_put(LED_PIN, 1);
        sleep_ms(100);
        gpio_put(LED_PIN, 0);
        sleep_ms(100);
        gpio_put(LED_PIN, 1);
        sleep_ms(100);
        gpio_put(LED_PIN, 0);
        sleep_ms(100);
        P0 = false;
        PONG_State = SERVE;
      break;
  }
}
