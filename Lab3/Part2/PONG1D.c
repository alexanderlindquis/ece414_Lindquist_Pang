// Main module for PONG1D Game Logic

//Include necessary modules/libraries
#include "sw_in.h"

static enum PONG_States (LEFT, RIGHT, SERVE, BLINK} PONG_State;
const uint32_t MASK_9_2 = 0x000003fc;
uint32_t outval = 0x1;
bool P0 = false;
//main logic
int main(){
  tBtn = timer_read();
  tPong = timer_read();
  uint32 PONGPD_MS = 300;
  gpio_init_mask(MASK_9_2);
  gpio_set_dir_out_masked(MASK_9_2);
  while(true){
    tCurrent = timer_read();
    gpio_put_masked(MASK_9_2, outval << 2);
    if(timer_elapsed_ms(tBtn, tCurrent) >= DEBOUNCEPD_MS){
      debounce_sw1_tick();
      deounce_sw2_tick();
    }
    if(debounce_sw1_pressed() || debounce_sw2_pressed() || timer_elapsed_ms(tPong, tCurrent) >= PONGPD_MS){
      pong_tick();
    }
  }
}

//Update Game State based on button presses, current active player, 
void pong_tick(){
  switch(PONG_State){
    case SERVE:
      if(P0 && debounce_sw1_pressed()){
        PONG_State = RIGHT;
        P0 = false;
      }
      else if(!P0 && debounce_sw2_pressed()){
        PONG_State = RIGHT;
        P0 = true;
      }
      break;
    case LEFT:
      if(debounce_sw1_pressed()){
        if(outval == 0x80){
          PONG_State = RIGHT;
          P0 = false;
          outval = outval >> 1;
        }
        else{
          PONG_State = BLINK;
        }
      }
      else if(outval <= 0x80){
        outval = outval << 1;
      }
      else{
        PONG_State = BLINK;
      }
      break;
    case RIGHT:
      if(debounce_sw2_pressed()){
        if(outval == 0x1){
          PONG_State = LEFT;
          P0 = true;
          outval = outval << 1;
        }
        else{
          PONG_State = BLINK;
        }
      }
      else if(outval >= 0x1){
        outval = outval >> 1;
      }
      else{
        PONG_State = BLINK;
      }
      break;
    case BLINK:
      if(P0){
        outval = 0x1;
      }
      else{
        outval = 0x80;
      }
      for(int i = 0; i < 10; i++){
        if(i % 2 == 0){
          gpio_put_masked(MASK_9_2, outval << 2);
        }
        else{
          gpio_put_masked(MASK_9_2, 0x0);
        }
        sleep_ms(100);
      }
      P0 = !P0;
      PONG_State = SERVE;
      break;
  }
}
