/**
  ******************************************************************************
  * @file    main.c
  * @author  Andrew Dile, Abhi Annigeri, Daniel Wang, William Lee
  * @date    November 4 2024
  * @brief   ECE 362 Snake Game
  ******************************************************************************
*/

#include "superSnake.h"
#include "helper.h"

int main() {
  // set ups
  internal_clock();
  init_usart5();
  setupLCDDisplay();
  initializeSnake();
  setupDMA();
  enableDMA();
  // setupJoystick();
  //for oled

  init_spi1();
  spi1_init_oled();

  //test oled
  spi1_dma_display1("where r u");
  spi1_dma_display2("will");

  // loop part of game
  while (true) {
    updateLCDDisplay();

    updateJoystick();
  }
}
