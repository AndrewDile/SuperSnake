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
  setup_adc();
  init_tim2();
  setupLCDDisplay();
  initializeSnake();
  setupMovementTimer();

  mountSD();
  writeHighScoresToSD();
  readHighScoresFromSD();

  setupOLED();

  setup_tim1();

  playSound(1);

  // loop part of game
  while (true) {
    updateLCDDisplay();

    updateJoystick();

    updateOLED();
  }
}
