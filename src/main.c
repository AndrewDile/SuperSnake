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
  test_oled();


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



void test_oled(void) {
    // Initialize SPI and OLED
    init_spi1();
    spi1_init_oled();
    
    // Test pattern 1: Static text
    oled_write_string("OLED Test", 0);
    oled_write_string("Line 2 Test", 1);
    nano_wait(2000000000); // Wait 2 seconds
    
    // Test pattern 2: Counting
    char buf[16];
    for(int i = 0; i < 10; i++) {
        sprintf(buf, "Count: %d", i);
        oled_write_string(buf, 0);
        oled_write_string("Counting...", 1);
        nano_wait(1000000000); // Wait 1 second
    }
    
    // Test pattern 3: All characters
    for(char c = '!'; c <= 'Z'; c++) {
        buf[0] = c;
        buf[1] = '\0';
        oled_write_string(buf, 0);
        nano_wait(500000000); // Wait 0.5 seconds
    }
}