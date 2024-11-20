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

  // loop part of game
  while (true) {
    updateLCDDisplay();

    updateJoystick();
  }
}