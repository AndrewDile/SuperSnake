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
  // setupDMA();
  // enableDMA();
  setupJoystick();

  mountSD();
  writeHighScoresToSD();
  readHighScoresFromSD();

  setupOLED();
  updateOLED(HIGHS);

  setup_tim1();

  playSound(1);

  // while (true) {
  //   // Check direction and act accordingly
  //   if (direction == 'L') {
  //     joystickDirection = LEFT;
  //   } else if (direction == 'R') {
  //     joystickDirection = RIGHT;
  //   } else {
  //     joystickDirection = NEUTRAL;
  //   }
  // }

  // loop part of game
  while (true) {
    updateLCDDisplay();

    updateJoystick();
  }
}
