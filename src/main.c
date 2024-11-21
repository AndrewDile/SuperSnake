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

int8_t highscore1 = 101;
int8_t highscore2 = 70;
int8_t highscore3 = 30;
int8_t highscore4 = 6;

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

  setup_tim1();

  playSound(1);

  //test oled
  spi1_dma_display1("where r u");
  spi1_dma_display2("will");

  // FIL fil;
  // FRESULT fr;
  // fr = f_open(&fil, "highscores.txt", FA_WRITE | FA_CREATE_NEW);
  // if (fr != FR_OK) return;
  // UINT wlen;
  // int8_t highscores[4] = {highscore1, highscore2, highscore3, highscore4};
  // fr = f_write(&fil, (BYTE*)highscores, 4, wlen);
  // if (!fr) {
  //   LCD_Clear(purple);
  // }

  // loop part of game
  while (true) {
    // updateLCDDisplay();

    updateJoystick();
  }
}
