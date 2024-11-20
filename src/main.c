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
  init_oled_gpio();
  init_oled();


  oled_write_two_lines("Snake Game", "Press to Start");

  gameboard[5][5] = BEND_DOWN_RIGHT;
  gameboard[6][5] = BEND_DOWN_LEFT;
  gameboard[5][6] = BEND_UP_RIGHT;
  gameboard[6][6] = BEND_UP_LEFT;

  gameboard[0][19] = HEAD_UP;

  // loop part of game
  while (true) {
    updateLCDDisplay();

    updateJoystick();
  }
}