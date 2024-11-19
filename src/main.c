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
  // setupJoystick();

  // loop part of game
  while (true) {
    updateLCDDisplay();

    updateJoystick();

    // switch (joystickDirection) {
    //   case NEUTRAL:
    //     LCD_Clear(white);
    //     break;
    //   case UP:
    //     LCD_Clear(red);
    //     break;
    //   case DOWN:
    //     LCD_Clear(orange);
    //     break;
    //   case RIGHT:
    //     LCD_Clear(green);
    //     break;
    //   case LEFT:
    //     LCD_Clear(blue);
    //     break;
    //   default:
    //     LCD_Clear(purple);
    //     break;
    // }
  }
}