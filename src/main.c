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

 if(lastGameState != gameState) {
        switch(gameState) {
            case IDLE:
                oled_write_two_lines("Snake Game","Ready!");
                break;
            case RUNNING:
                oled_write_two_lines("Score:","Playing...");
                break;
            case GAMELOST:
                oled_write_two_lines("Game Over","Press to Reset");
                break;
            case GAMEWON:
                oled_write_two_lines("You Won!","Press to Reset");
                break;
        }
        lastGameState = gameState;

  char score_str[16];
        char state_str[16];
        
        sprintf(score_str, "Score: %d", snakeLength - INITIAL_SNAKE_LENGTH);
        
        switch(gameState) {
            case IDLE:
                sprintf(state_str, "Press to Start");
                break;
            case RUNNING:
                sprintf(state_str, "Playing...");
                break;
            case GAMELOST:
                sprintf(state_str, "Game Over!");
                break;
            case GAMEWON:
                sprintf(state_str, "You Won!");
                break;
        }
        
        oled_write_two_lines(score_str, state_str);

  }
}