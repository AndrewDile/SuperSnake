/**
 ******************************************************************************
 * @file    superSnake.c
 * @author  Andrew Dile, Abhi Annigeri, Daniel Wang, William Lee
 * @date    November 4 2024
 * @brief   ECE 362 Snake Game
 ******************************************************************************
 */

#include "superSnake.h"
#include "fifo.h"
#include "tty.h"

// variables for game state handling
int8_t gameState = IDLE;
int8_t lastGameState = IDLE;

// gameboard is 2D array of gametile structures, value of tile updated on snake movement
int8_t gameboard[NUM_X_CELLS][NUM_Y_CELLS] = {0}; // value determines what is displayed in that cell (0 = blank, 1 = snack, 2 = head facing left, etc.)

// array to store snake
segment snake[NUM_X_CELLS * NUM_Y_CELLS]; // not dynamically stored to avoid fragmentation and leaks, and to ensure program doesn't run out of space during gameplay

// snake movement variables
int8_t snakeLength = 2;
uint32_t snakeSpeed = 1000;

// variable for joystick direction
int8_t joystickDirection = NEUTRAL;
int8_t joystickXraw = -1;
int8_t joystickYraw = -1;

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
      switch (gameboard[x][y]) {
      case EMPTY:
        LCD_DrawFillRectangle((CELL_PIXEL_WIDTH * x), Y_BORDER + (CELL_PIXEL_WIDTH * y), ((CELL_PIXEL_WIDTH * x) + CELL_PIXEL_WIDTH), (Y_BORDER + (CELL_PIXEL_WIDTH * y) + CELL_PIXEL_WIDTH), black);
        break;
      case SNACK:
        LCD_DrawFillRectangle((CELL_PIXEL_WIDTH * x) + 2, Y_BORDER + (CELL_PIXEL_WIDTH * y) + 2, ((CELL_PIXEL_WIDTH * x) + CELL_PIXEL_WIDTH) - 4, (Y_BORDER + (CELL_PIXEL_WIDTH * y) + CELL_PIXEL_WIDTH) - 4, green);
        break;
      case HEAD_LEFT:
        LCD_DrawFillRectangle((CELL_PIXEL_WIDTH * x), Y_BORDER + (CELL_PIXEL_WIDTH * y), ((CELL_PIXEL_WIDTH * x) + CELL_PIXEL_WIDTH), (Y_BORDER + (CELL_PIXEL_WIDTH * y) + CELL_PIXEL_WIDTH), orange);
        break;
      case HEAD_RIGHT:
        LCD_DrawFillRectangle((CELL_PIXEL_WIDTH * x), Y_BORDER + (CELL_PIXEL_WIDTH * y), ((CELL_PIXEL_WIDTH * x) + CELL_PIXEL_WIDTH), (Y_BORDER + (CELL_PIXEL_WIDTH * y) + CELL_PIXEL_WIDTH), orange);
        break;
      case HEAD_UP:
        LCD_DrawFillRectangle((CELL_PIXEL_WIDTH * x) + 1, Y_BORDER + (CELL_PIXEL_WIDTH * y) + 1, ((CELL_PIXEL_WIDTH * x) + CELL_PIXEL_WIDTH) - 2, (Y_BORDER + (CELL_PIXEL_WIDTH * y) + CELL_PIXEL_WIDTH), orange);
        break;
      case HEAD_DOWN:
        LCD_DrawFillRectangle((CELL_PIXEL_WIDTH * x), Y_BORDER + (CELL_PIXEL_WIDTH * y), ((CELL_PIXEL_WIDTH * x) + CELL_PIXEL_WIDTH), (Y_BORDER + (CELL_PIXEL_WIDTH * y) + CELL_PIXEL_WIDTH), orange);
        break;
      case SEGMENT_VER:
        LCD_DrawFillRectangle((CELL_PIXEL_WIDTH * x) + 1, Y_BORDER + (CELL_PIXEL_WIDTH * y), ((CELL_PIXEL_WIDTH * x) + CELL_PIXEL_WIDTH) - 2, (Y_BORDER + (CELL_PIXEL_WIDTH * y) + CELL_PIXEL_WIDTH), orange);
        break;
      case SEGMENT_HOR:
        LCD_DrawFillRectangle((CELL_PIXEL_WIDTH * x), Y_BORDER + (CELL_PIXEL_WIDTH * y) + 1, ((CELL_PIXEL_WIDTH * x) + CELL_PIXEL_WIDTH), (Y_BORDER + (CELL_PIXEL_WIDTH * y) + CELL_PIXEL_WIDTH) - 2, orange);
        break;
      case BEND_UP_RIGHT:
        LCD_DrawFillRectangle((CELL_PIXEL_WIDTH * x), Y_BORDER + (CELL_PIXEL_WIDTH * y), ((CELL_PIXEL_WIDTH * x) + CELL_PIXEL_WIDTH), (Y_BORDER + (CELL_PIXEL_WIDTH * y) + CELL_PIXEL_WIDTH), orange);
        break;
      case BEND_UP_LEFT:
        LCD_DrawFillRectangle((CELL_PIXEL_WIDTH * x), Y_BORDER + (CELL_PIXEL_WIDTH * y), ((CELL_PIXEL_WIDTH * x) + CELL_PIXEL_WIDTH), (Y_BORDER + (CELL_PIXEL_WIDTH * y) + CELL_PIXEL_WIDTH), orange);
        break;
      case BEND_DOWN_RIGHT:
        LCD_DrawFillRectangle((CELL_PIXEL_WIDTH * x), Y_BORDER + (CELL_PIXEL_WIDTH * y), ((CELL_PIXEL_WIDTH * x) + CELL_PIXEL_WIDTH), (Y_BORDER + (CELL_PIXEL_WIDTH * y) + CELL_PIXEL_WIDTH), orange);
        break;
      case BEND_DOWN_LEFT:
        LCD_DrawFillRectangle((CELL_PIXEL_WIDTH * x), Y_BORDER + (CELL_PIXEL_WIDTH * y), ((CELL_PIXEL_WIDTH * x) + CELL_PIXEL_WIDTH), (Y_BORDER + (CELL_PIXEL_WIDTH * y) + CELL_PIXEL_WIDTH), orange);
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


  RCC->CR2 |= RCC_CR2_HSI14ON; //enable highspeed 14MHz

  while (!(RCC->CR2 & RCC_CR2_HSI14RDY)); //wait


  // set resolution to 6 bits

  ADC1->CFGR1 &= ~ADC_CFGR1_RES;

  ADC1->CFGR1 |= ADC_CFGR1_RES_1;


  // configure ADC to only do a conversion after the last value has been read

  ADC1->CFGR1 |= ADC_CFGR1_WAIT;


 // enable ADC peripheral and wait for it to be ready

  ADC1->CR |= ADC_CR_ADEN;

  while (!(ADC1->ISR & ADC_ISR_ADRDY));

  while((ADC1->CR & ADC_CR_ADSTART));


  // select channels 0 and 1 (PA0 for JoystickX, PA1 for JoystickY)

  ADC1->CHSELR = ADC_CHSELR_CHSEL0 | ADC_CHSELR_CHSEL1;


  // enable end of conversion interrupt

  ADC1->IER |= ADC_IER_EOCIE;


  // start conversion

  ADC1->CR |= ADC_CR_ADSTART;


  // enable ADC interrupt in NVIC

  NVIC_EnableIRQ(ADC1_IRQn);
}

void updateJoystick() {
  int8_t ADCReading = ADC1->DR;
  // read values sequentially

  if(joystickXraw == -1) {
    joystickXraw = ADCReading;

  } else {
    joystickYraw = ADCReading;

    // set joystickDirection based on readings
    if (joystickXraw < 10) joystickDirection = LEFT;
    else if (joystickXraw > 50) joystickDirection = RIGHT;
    else if (joystickYraw < 10) joystickDirection = DOWN;
    else if (joystickYraw > 50) joystickDirection = UP;
    else joystickDirection = NEUTRAL;

    // prevent 180 degree turns during game
    if (gameState == RUNNING) {
      switch (joystickDirection) {
        case NEUTRAL:
          break;
        case UP:
          if (snake[0].direction == DOWN) break;
          snake[0].direction = joystickDirection;
          break;
        case RIGHT:
          if (snake[0].direction == LEFT) break;
          snake[0].direction = joystickDirection;
          break;
        case DOWN:
          if (snake[0].direction == UP) break;
          snake[0].direction = joystickDirection;
          break;
        case LEFT:
          if (snake[0].direction == RIGHT) break;
          snake[0].direction = joystickDirection;
          break;
        default:
        break;
      }
    }
    joystickXraw = -1;
    joystickYraw = -1;
  }
}


void setupDMA(){

  RCC->AHBENR |= RCC_AHBENR_DMA1EN;

  ADC1->CFGR1 |= ADC_CFGR1_DMAEN | ADC_CFGR1_DMACFG;

  DMA1_Channel1->CPAR = (uint32_t) (&(ADC1->DR));

  DMA1_Channel1->CMAR = (uint32_t)(joystickDirection);

  DMA1_Channel1->CNDTR = 2;

  DMA1_Channel1->CCR |= DMA_CCR_MINC | DMA_CCR_MSIZE_0 | DMA_CCR_PSIZE_0 | DMA_CCR_TEIE | DMA_CCR_CIRC;

  DMA1_Channel1->CCR |= DMA_CCR_EN;

}


void enableDMA(void){

  DMA1_Channel1->CCR |= DMA_CCR_EN;

}

void initializeSnake() {
  // set position and direction of snake head and first two segments
  snake[0].x = NUM_X_CELLS / 2;
  snake[0].y = NUM_Y_CELLS / 2;
  snake[0].direction = UP;
  snake[1].x = snake[0].x;
  snake[1].y = snake[0].y + 1;
  snake[1].direction = snake[0].direction;
  snake[2].x = snake[0].x;
  snake[2].y = snake[1].y + 1;
  snake[2].direction = snake[0].direction;

  gameboard[snake[0].x][snake[0].y] = HEAD_UP;
  gameboard[snake[1].x][snake[1].y] = SEGMENT_VER;
  gameboard[snake[2].x][snake[2].y] = SEGMENT_VER;
  
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

  for (int i = 0; i < snakeLength && snakeLength < NUM_X_CELLS * NUM_Y_CELLS; i++) {
    // snake head logic
    if (i == 1) {
      // update position, detect border collision, detect eating, detect winning, update gameboard
      switch (snake[0].direction) {
        case UP:
          snake[0].y--;
          if (snake[0].y < 0 || snake[0].y >= NUM_Y_CELLS) {  // border collision
            gameState = GAMELOST;
            playSound(LOST);
            return;
          }
          if (gameboard[snake[0].x][snake[0].y] == SNACK) {  // eating detection
            snakeLength++;
            if (snakeLength >= NUM_X_CELLS * NUM_Y_CELLS) {  // winning detection
              gameState = GAMEWON;
              playSound(WON);
              return;
            }
            ateSnack();  // possibly obtain an ability
            playSound(EAT);
            generateSnack();  // generate new snack
          }
          gameboard[snake[0].x][snake[0].y] = HEAD_UP; // update gameboard
          break;
        case DOWN:
          snake[0].y++;
          if (snake[0].y < 0 || snake[0].y >= NUM_Y_CELLS) {  // border collision
            gameState = GAMELOST;
            playSound(LOST);
            return;
          }
          if (gameboard[snake[0].x][snake[0].y] == SNACK) {  // eating detection
            snakeLength++;
            if (snakeLength >= NUM_X_CELLS * NUM_Y_CELLS) {  // winning detection
              gameState = GAMEWON;
              playSound(WON);
              return;
            }
            ateSnack();  // possibly obtain an ability
            playSound(EAT);
            generateSnack();  // generate new snack
          }
          gameboard[snake[0].x][snake[0].y] = HEAD_DOWN; // update gameboard
          break;
        case LEFT:
          snake[0].x--;
          if (snake[0].x < 0 || snake[0].x >= NUM_X_CELLS) {  // border collision
            gameState = GAMELOST;
            playSound(LOST);
            return;
          }
          if (gameboard[snake[0].x][snake[0].y] == SNACK) {  // eating detection
            snakeLength++;
            if (snakeLength >= NUM_X_CELLS * NUM_Y_CELLS) {  // winning detection
              gameState = GAMEWON;
              playSound(WON);
              return;
            }
            ateSnack();  // possibly obtain an ability
            playSound(EAT);
            generateSnack();  // generate new snack
          }
          gameboard[snake[0].x][snake[0].y] = HEAD_LEFT; // update gameboard
          break;
        case RIGHT:
          snake[0].x++;
          if (snake[0].x < 0 || snake[0].x >= NUM_X_CELLS) {  // border collision
            gameState = GAMELOST;
            playSound(LOST);
            return;
          }
          if (gameboard[snake[0].x][snake[0].y] == SNACK) {  // eating detection
            snakeLength++;
            if (snakeLength >= NUM_X_CELLS * NUM_Y_CELLS) {  // winning detection
              gameState = GAMEWON;
              playSound(WON);
              return;
            }
            ateSnack();  // possibly obtain an ability
            playSound(EAT);
            generateSnack();  // generate new snack
          }
          gameboard[snake[0].x][snake[0].y] = HEAD_RIGHT; // update gameboard
          break;
      }
    }

    // snake segment logic
    else if (i > 0 && i != snakeLength - 1) {
      // if new segment, initialize properly based on pieces ahead of it
      if (snake[i].direction == NEUTRAL) {

      }

      // update position
      switch (snake[i].direction) {
        case UP:
          snake[i].y--;
          break;
        case DOWN:
          snake[i].y++;
          break;
        case LEFT:
          snake[i].x--;
          break;
        case RIGHT:
          snake[i].x++;
          break;
        default:
          break;
      }

      // obtain last direction before updating it
      int8_t lastDirection = snake[i].direction;

      // update direction
      if (snake[i].x - snake[i - 1].x > 0) {
        snake[i].direction = LEFT;
      } else {
        snake[i].direction = RIGHT;
      }
      if (snake[i].y - snake[i - 1].y > 0) {
        snake[i].direction = UP;
      } else {
        snake[i].direction = DOWN;
      }

      // update gameboard
      switch (lastDirection) {
        case UP:
          switch (snake[i].direction) {
            case UP:
              gameboard[snake[i].x][snake[i].y] = SEGMENT_VER;
              break;
            case RIGHT:
              gameboard[snake[i].x][snake[i].y] = BEND_RIGHT_DOWN;
              break;
            case LEFT:
              gameboard[snake[i].x][snake[i].y] = BEND_LEFT_DOWN;
              break;
            default:
              break;
          }
          break;
        case DOWN:
          switch (snake[i].direction) {
            case DOWN:
              gameboard[snake[i].x][snake[i].y] = SEGMENT_VER;
              break;
            case RIGHT:
              gameboard[snake[i].x][snake[i].y] = BEND_RIGHT_UP;
              break;
            case LEFT:
              gameboard[snake[i].x][snake[i].y] = BEND_LEFT_UP;
              break;
            default:
              break;
          }
          break;
        case RIGHT:
          switch (snake[i].direction) {
            case UP:
              gameboard[snake[i].x][snake[i].y] = BEND_UP_LEFT;
              break;
            case DOWN:
              gameboard[snake[i].x][snake[i].y] = BEND_DOWN_LEFT;
              break;
            case RIGHT:
              gameboard[snake[i].x][snake[i].y] = SEGMENT_HOR;
              break;
            default:
              break;
          }
          break;
        case LEFT:
          switch (snake[i].direction) {
            case UP:
              gameboard[snake[i].x][snake[i].y] = BEND_UP_RIGHT;
              break;
            case DOWN:
              gameboard[snake[i].x][snake[i].y] = BEND_DOWN_RIGHT;
              break;
            case LEFT:
              gameboard[snake[i].x][snake[i].y] = SEGMENT_HOR;
              break;
            default:
              break;
          }
          break;
      }
    }

    // snake tail logic
    else {
      // update position
      switch (snake[i].direction) {
        case UP:
          snake[i].y--;
          break;
        case DOWN:
          snake[i].y++;
          break;
        case LEFT:
          snake[i].x--;
          break;
        case RIGHT:
          snake[i].x++;
          break;
        default:
          break;
      }

      // obtain last direction before updating it
      int8_t lastDirection = snake[i].direction;

      // update direction
      if (snake[i].x - snake[i - 1].x > 0) {
        snake[i].direction = LEFT;
      } else {
        snake[i].direction = RIGHT;
      }
      if (snake[i].y - snake[i - 1].y > 0) {
        snake[i].direction = UP;
      } else {
        snake[i].direction = DOWN;
      }

      // update gameboard
      
    }
  }

  // now that full snake is updated, check for collision with self
  for (int i = 0; i < snakeLength; i++) {
    if (snake[0].x == snake[i].x && snake[0].y == snake[i].y) {
      gameState = GAMELOST;
      playSound(LOST);
      return;
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

// 
void ateSnack() {

}