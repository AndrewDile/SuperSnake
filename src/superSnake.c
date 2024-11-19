/**
 ******************************************************************************
 * @file    superSnake.c
 * @author  Andrew Dile, Abhi Annigeri, Daniel Wang, William Lee
 * @date    November 4 2024
 * @brief   ECE 362 Snake Game
 ******************************************************************************
 */

#include "superSnake.h"
#include "stm32f0xx.h"
#include <stdint.h>
#include <stdio.h>
#include "fifo.h"
#include "tty.h"

// variables for game state handling
int8_t gameState = IDLE;
int8_t lastGameState = IDLE;

// gameboard is 2D array of gametile structures, value of tile updated on snake movement
uint8_t gameboard[NUM_X_CELLS][NUM_Y_CELLS] = {0}; // value determines what is displayed in that cell (0 = blank, 1 = snack, 2 = head facing left, etc.)

// array to store snake
segment snake[NUM_X_CELLS * NUM_Y_CELLS]; // not dynamically stored to avoid fragmentation and leaks, and to ensure program doesn't run out of space during gameplay

// variable for joystick direction
int8_t joystickDirection = NEUTRAL;

// flag for some code to only run on first cycle
bool initialized = false;

// color values
uint16_t white = 65535;
uint16_t black = 0;
uint16_t red = 63488;
uint16_t green = 2016;
uint16_t blue = 31;
uint16_t yellow = 65504;
uint16_t purple = 30735;
uint16_t orange = 64512;

// access memory via SPI to SD interface to get high score memory
void readMemory() {

}

// set up LCD display to be communicated with
void setupLCDDisplay() {
  LCD_Setup();
  LCD_Clear(white);
  LCD_DrawFillRectangle(0, Y_BORDER, X_PIXELS, Y_PIXELS - Y_BORDER, black);
}

// function that updates LCD display
void updateLCDDisplay() {
  for (int x = 0; x <= NUM_X_CELLS; x++) {
    for (int y = 0; y <= NUM_Y_CELLS; y++) {
      switch (gameboard[x][y])
      {
      case EMPTY:
        LCD_DrawFillRectangle(X_BORDER + (CELL_PIXEL_WIDTH * x), Y_BORDER + (CELL_PIXEL_WIDTH * y), (X_BORDER + (CELL_PIXEL_WIDTH * x) + CELL_PIXEL_WIDTH), (Y_BORDER + (CELL_PIXEL_WIDTH * y) + CELL_PIXEL_WIDTH), green);
        break;

      case SNACK:
        
        break;

      case HEAD_LEFT:
        
        break;

      case HEAD_RIGHT:
        
        break;

      case HEAD_UP:
        
        break;

      case HEAD_DOWN:
        
        break;

      case SEGMENT_VER:
        
        break;

      case SEGMENT_HOR:
        
        break;

      case BEND_UP_RIGHT:
        
        break;

      case BEND_UP_LEFT:
        
        break;

      case BEND_DOWN_RIGHT:
        
        break;

      case BEND_DOWN_LEFT:
        
        break;

      default:
        break;
      }
    }
  }
}

// void init_gpio() {
//     RCC->AHBENR |= (RCC_AHBENR_GPIOAEN | RCC_AHBENR_GPIOBEN);
//     GPIOA->MODER |= (GPIO_MODER_MODER0 | GPIO_MODER_MODER1); // Joystick PA0, PA1 as analog
//     GPIOB->MODER &= ~(GPIO_MODER_MODER0 | GPIO_MODER_MODER1); // LED PB0, PB1 clear mode
//     GPIOB->MODER |= (GPIO_MODER_MODER0_0 | GPIO_MODER_MODER1_0); // Set PB0, PB1 as output
//     GPIOB->OSPEEDR |= (GPIO_OSPEEDER_OSPEEDR0 | GPIO_OSPEEDER_OSPEEDR1); // High speed for LEDs
//     GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPDR0 | GPIO_PUPDR_PUPDR1); // No pull-up or pull-down for LEDs
// }

// function to initialize ADC for joystick readings
void setupJoystick() {
  // enable RCC clocks, port A, and pins
  RCC->APB2ENR |= RCC_APB2ENR_ADCEN;
  RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
  GPIOA->MODER |= 0xF;

  // set resolution to 6 bits
  ADC1->CFGR1 &= ~ADC_CFGR1_RES;
  ADC1->CFGR1 |= ADC_CFGR1_RES_1;

  // configure ADC to only do a conversion after the last value has been read
  ADC1->CFGR1 |= ADC_CFGR1_WAIT;

  // select channels 0 and 1 (PA0 for JoystickX, PA1 for JoystickY)
  ADC1->CHSELR = 0x3;

  // enable end of conversion interrupt
  ADC1->IER |= ADC_IER_EOCIE;

  // enable ADC peripheral and wait for it to be ready
  ADC1->CR |= ADC_CR_ADEN;
  while (!(ADC1->ISR & ADC_ISR_ADRDY));

  // start conversion
  ADC1->CR |= ADC_CR_ADSTART;

  // enable ADC interrupt in NVIC
  NVIC_EnableIRQ(ADC1_IRQn);
}

// IRQ Handler when ADC conversion finishes
void ADC1_IRQHandler() {
  // initialize temp variables
  static int8_t x = -1;
  static int8_t y = -1;

  // read values sequentially
  if (ADC1->ISR & ADC_ISR_EOC) {
    if(x == -1) {
      x = ADC1->DR;
    } else {
      y = ADC1->DR;

      // set joystickDirection based on readings
      if (x < 10) joystickDirection = LEFT;
      else if (x > 50) joystickDirection = RIGHT;
      else if (y < 10) joystickDirection = DOWN;
      else if (y > 50) joystickDirection = UP;
      else joystickDirection = NEUTRAL;

      snake[0].direction = joystickDirection;

      // start new ADC conversion
      ADC1->CR |= ADC_CR_ADSTART;
      x = -1;
      y = -1;
    }
  }
}

void initializeSnake() {
    // Start snake in middle of board
    // uint8_t startX = NUM_X_CELLS / 2;
    // uint8_t startY = NUM_Y_CELLS / 2;
    
    // // Initialize snake segments
    // for(int i = 0; i < snakeLength; i++) {
    //     snake[i].x = startX - i;  // Snake starts horizontally
    //     snake[i].y = startY;
    //     gameboard[startX - i][startY] = (i == 0) ? 2 : 3;  // 2 for head, 3 for body
    // }
    
    // Generate first snack
    generateSnack();
}

void setupMovementTimer() {
    // Enable TIM3 clock
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
    
    // Set prescaler for 1ms resolution
    TIM3->PSC = 47999;  // 48MHz/48000 = 1kHz
    
    // Set initial period to snakeSpeed (in ms)
    TIM3->ARR = INITIAL_SNAKE_SPEED;
    
    // Enable update interrupt
    TIM3->DIER |= TIM_DIER_UIE;
    
    // Enable timer
    TIM3->CR1 |= TIM_CR1_CEN;
    
    // Enable TIM3 interrupt in NVIC
    NVIC_EnableIRQ(TIM3_IRQn);
}

void generateSnack() {
  // Generate new snack at random position
  // Create arrays to store empty positions
  uint8_t emptyX[NUM_X_CELLS * NUM_Y_CELLS];
  uint8_t emptyY[NUM_X_CELLS * NUM_Y_CELLS];
  uint16_t emptyCount = 0;
  
  // Find all empty positions on the gameboard
  for(uint8_t x = 0; x < NUM_X_CELLS; x++) {
    for(uint8_t y = 0; y < NUM_Y_CELLS; y++) {
      if(gameboard[x][y] == EMPTY) {
        emptyX[emptyCount] = x;
        emptyY[emptyCount] = y;
        emptyCount++;
      }
    }
  }
    
  // Check if there are any empty spaces
  if(emptyCount == 0) {
    // Game is won - no empty spaces left
    gameState = GAMEWON;
    return;
  }
  
  // Generate random index from available empty positions
  uint16_t randomIndex = rand() % emptyCount;
  
  // Get the randomly selected empty position
  uint8_t newSnackX = emptyX[randomIndex];
  uint8_t newSnackY = emptyY[randomIndex];
  
  // Place snack on gameboard
  gameboard[newSnackX][newSnackY] = SNACK;
}

void movementLogic() {
  // Return if game isn't running
  if(gameState != RUNNING) return;
  
  // Calculate new head position based on current head position
  uint8_t newHeadX = snake[0].x;
  uint8_t newHeadY = snake[0].y;
  uint8_t oldDirection = snake[0].direction;
  
  // Update position based on current joystick direction
  // joystickDirection i believe is already being updated by ADC1_IRQHandler
  switch(joystickDirection) {
  case UP:
    if(oldDirection != DOWN) { // Prevent 180-degree turns
      newHeadY = (newHeadY > 0) ? newHeadY - 1 : NUM_Y_CELLS - 1;
      snake[0].direction = UP;
    }
    break;
  case DOWN:
    if(oldDirection != UP) {
      newHeadY = (newHeadY + 1) % NUM_Y_CELLS;
      snake[0].direction = DOWN;
    }
    break;
  case LEFT:
    if(oldDirection != RIGHT) {
      newHeadX = (newHeadX > 0) ? newHeadX - 1 : NUM_X_CELLS - 1;
      snake[0].direction = LEFT;
    }
    break;
  case RIGHT:
    if(oldDirection != LEFT) {
      newHeadX = (newHeadX + 1) % NUM_X_CELLS;
      snake[0].direction = RIGHT;
    }
    break;
  case NEUTRAL:
    // Continue in current direction
    switch(oldDirection) {
      case UP:
        newHeadY = (newHeadY > 0) ? newHeadY - 1 : NUM_Y_CELLS - 1;
        break;
      case DOWN:
        newHeadY = (newHeadY + 1) % NUM_Y_CELLS;
        break;
      case LEFT:
        newHeadX = (newHeadX > 0) ? newHeadX - 1 : NUM_X_CELLS - 1;
        break;
      case RIGHT:
        newHeadX = (newHeadX + 1) % NUM_X_CELLS;
        break;
    }
    break;
  }
  
  // Check for collision with self
  for(int i = 1; i < snakeLength; i++) {
    if(newHeadX == snake[i].x && newHeadY == snake[i].y) {
      gameState = GAMELOST;
      playSound(LOST);
      return;
    }
  }
  
  // Check if snack was eaten
  bool snackEaten = (gameboard[newHeadX][newHeadY] == SNACK);
  
  // Store old positions for body movement
  uint8_t prevX[MAX_SNAKE_LENGTH];
  uint8_t prevY[MAX_SNAKE_LENGTH];
  uint8_t prevDir[MAX_SNAKE_LENGTH];
  
  for(int i = 0; i < snakeLength; i++) {
    prevX[i] = snake[i].x;
    prevY[i] = snake[i].y;
    prevDir[i] = snake[i].direction;
    // Clear current position on gameboard
    gameboard[snake[i].x][snake[i].y] = EMPTY;
  }
  
  // Update head position
  snake[0].x = newHeadX;
  snake[0].y = newHeadY;
  
  // Set head tile type based on direction
  switch(snake[0].direction) {
    case LEFT:
      gameboard[newHeadX][newHeadY] = HEAD_LEFT;
      break;
    case RIGHT:
      gameboard[newHeadX][newHeadY] = HEAD_RIGHT;
      break;
    case UP:
      gameboard[newHeadX][newHeadY] = HEAD_UP;
      break;
    case DOWN:
      gameboard[newHeadX][newHeadY] = HEAD_DOWN;
      break;
  }
  
  // Update body segments
  for(int i = 1; i < snakeLength; i++) {
    snake[i].x = prevX[i-1];
    snake[i].y = prevY[i-1];
    snake[i].direction = prevDir[i-1];
    
    // Determine segment tile type
    if(snake[i].direction == snake[i-1].direction) {
      // Straight segment
      gameboard[snake[i].x][snake[i].y] = (snake[i].direction == LEFT || snake[i].direction == RIGHT) ? SEGMENT_HOR : SEGMENT_VER;
    } else {
        // Bend segment
        if((snake[i-1].direction == RIGHT && snake[i].direction == UP) || (snake[i-1].direction == DOWN && snake[i].direction == LEFT)) {
          gameboard[snake[i].x][snake[i].y] = BEND_UP_LEFT;
        } else if((snake[i-1].direction == RIGHT && snake[i].direction == DOWN) || (snake[i-1].direction == UP && snake[i].direction == LEFT)) {
          gameboard[snake[i].x][snake[i].y] = BEND_DOWN_LEFT;
        } else if((snake[i-1].direction == LEFT && snake[i].direction == UP) || (snake[i-1].direction == DOWN && snake[i].direction == RIGHT)) {
          gameboard[snake[i].x][snake[i].y] = BEND_UP_RIGHT;
        } else {
          gameboard[snake[i].x][snake[i].y] = BEND_DOWN_RIGHT;
        }
      }
  }
  
  if(snackEaten) {
    // Increase snake length and adjust speed
    if(snakeLength < MAX_SNAKE_LENGTH) {
      snakeLength++;
      if(snakeSpeed > MIN_SNAKE_SPEED) {
        snakeSpeed -= SPEED_INCREASE;
        // Update timer period
        TIM3->ARR = snakeSpeed;
      }
      playSound(EAT); // Play snack eaten sound
      generateSnack(); // Generate new snack
    }
  }
  
  // Update the game display
  updateLCDDisplay();
}

void TIM3_IRQHandler() {
  if(TIM3->SR & TIM_SR_UIF) {  // If update interrupt flag is set
    TIM3->SR &= ~TIM_SR_UIF;  // Clear interrupt flag
    movementLogic();          // Call movement logic
  }
}

void gameStateHandler() {
  if (lastGameState != gameState) {
    switch (gameState) {
      case IDLE:

        break;
      
      case RUNNING:

        break;
      
      case GAMELOST:

        break;

      case GAMEWON:

        break;
      
      default:
        break; 
    }
    lastGameState = gameState;
  }
}

// plays song based on value, maybe updates a flag once it finishes a song?
void playSound(uint8_t song) {
  switch(song) {
    case 0:
      TIM3->ARR = 999;
      break;
    case 1:
      TIM3->ARR = 799;
      break;
    case 2:
      TIM3->ARR = 599;
      break;
    default:
      TIM3->CCR1 = 0; //muted 
      break;
  }
  TIM3->CCR1 = TIM3->ARR / 2; //maintains 50% cycle 
}