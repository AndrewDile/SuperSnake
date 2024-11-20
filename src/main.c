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
  spi1_setup_dma();
  spi1_enable_dma();
  
  spi1_dma_display1();
  spi1_dma_display2();


  //test oled
  spi1_dma_display1("snake");
    spi1_dma_display2("game");


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
