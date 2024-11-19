/**
  ******************************************************************************
  * @file    superSnake.h
  * @author  Andrew Dile, Abhi Annigeri, Daniel Wang, William Lee
  * @date    November 4 2024
  * @brief   ECE 362 Snake Game
  ******************************************************************************
*/

#include "stm32f0xx.h"
#include <stdint.h>
#include <stdio.h>
#include "fifo.h"
#include "tty.h"
#include "commands.h"

// display is 240x320 pixels

//////////// PIN MAP ////////////
// 
//  PA1  as ADC1 -- JoystickX
//  PA2  as ADC2 -- JoystickY
//  PA3  as GPIO -- JoystickSw
//  
//  PC0  as GPIO -- LcdRS
//  PC1  as GPIO -- LcdRW
//  PC2  as GPIO -- LcdE
//  PC3  as GPIO -- LcdDb0
//  PC4  as GPIO -- LcdDb1
//  PC5  as GPIO -- LcdDb2
//  PC6  as GPIO -- LcdDb3
//  PC7  as GPIO -- LcdDb4
//  PC8  as GPIO -- LcdDb5
//  PC9  as GPIO -- LcdDb6
//  PC10 as GPIO -- LcdDb7
//
//  Using Lab7 Pin Schematic For
//    TFT Display and SD Card
//  UART also connected like past
//    labs
//
/////////////////////////////////

// global includes
#include "stm32f0xx.h"
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

// number of cells on the gameboard, can be changed but will drastically affect memory game takes to play
#define NUM_X_CELLS 20
#define NUM_Y_CELLS 20

// display constants
#define X_PIXELS 240  // 0 is left
#define Y_PIXELS 320  // 0 is top
#define CELL_PIXEL_WIDTH (X_PIXELS / NUM_X_CELLS)
#define X_BORDER 0
#define Y_BORDER ((Y_PIXELS - X_PIXELS) / 2)

// color hex code constants
extern uint16_t white;
extern uint16_t black;
extern uint16_t red;
extern uint16_t green;
extern uint16_t blue ;
extern uint16_t yellow;
extern uint16_t purple;
extern uint16_t orange;

// direction constants for code readability and standardization
#define NEUTRAL 0
#define RIGHT 1
#define LEFT 2
#define UP 3
#define DOWN 4

// game state constants for code readability and standardization
#define IDLE 0
#define RUNNING 1
#define GAMELOST 2
#define GAMEWON 3

// constants for tile pieces
#define EMPTY 0
#define SNACK 1
#define HEAD_LEFT 2
#define HEAD_RIGHT 3
#define HEAD_UP 4
#define HEAD_DOWN 5
#define SEGMENT_VER 6
#define SEGMENT_HOR 7
#define BEND_UP_RIGHT 8  // directions signify where the cell is connected to the snake segments (BEND_UP_RIGHT means the segment connects to a segments above and to the right)
#define BEND_UP_LEFT 9
#define BEND_DOWN_RIGHT 10
#define BEND_DOWN_LEFT 11
#define BEND_RIGHT_UP BEND_UP_RIGHT
#define BEND_RIGHT_DOWN BEND_DOWN_RIGHT
#define BEND_LEFT_UP BEND_UP_LEFT
#define BEND_LEFT_DOWN BEND_DOWN_LEFT
#define TAIL_LEFT 12  // direction signifies which way the tail connects to snake (TAIL_UP means next segment is above)
#define TAIL_RIGHT 13
#define TAIL_UP 14
#define TAIL_DOWN 15

// constants for audio
#define EAT 0
#define LOST 1
#define WON 2
#define ABILITY_USED 3

// constants for snake and movement
#define INITIAL_SNAKE_SPEED 500  // milliseconds between speed
#define INITIAL_SNAKE_LENGTH 3  // segments including head
#define MAX_SNAKE_LENGTH (NUM_X_CELLS * NUM_Y_CELLS)
#define MIN_SNAKE_SPEED 100      // maximum speed (minimum delay)
#define SPEED_INCREASE 25        // ms faster per snack eaten

#define PWM_MAX 2400

// each segment of the snake needs a position and a direction value
struct segment {
  int8_t x; // horizontal gametile position -- left = 0
  int8_t y; // vertical gametile position -- bottom = 0
  uint8_t direction; // direction current segment (or head) is facing
};

// defines the structure as a data type, for easier later use
typedef struct segment segment;

// exteral variables declared so other files to access
extern int8_t gameState;  // game over can be derived from this
extern int8_t lastGameState;
extern int8_t gameboard[NUM_X_CELLS][NUM_Y_CELLS];
extern segment snake[NUM_X_CELLS * NUM_Y_CELLS];
extern int8_t snakeLength; // current score can be derived from this
extern uint32_t snakeSpeed;
extern int8_t joystickDirection;
int8_t joystickXraw;
int8_t joystickYraw;

// function declarations
void setupLCDDisplay();
void updateLCDDisplay();
void setupJoystick();
void updateJoystick();
void initializeSnake();
void setupMovementTimer();
void generateSnack();
void movementLogic();
void gameStateHandler();
void playSound(uint8_t);