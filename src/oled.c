#include "superSnake.h"
#include "fifo.h"
#include "tty.h"

void init_spi1(void) {
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;        // Enable GPIOA 
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;       // Enable SPI1 

    // pins PA5(SCK), PA7(MOSI), PA15(NSS)
    //spi1
    //set using alternate func
    GPIOA->MODER &= ~(GPIO_MODER_MODER5 | GPIO_MODER_MODER7 | GPIO_MODER_MODER15);
    GPIOA->MODER |= (GPIO_MODER_MODER5_1 |   
                     GPIO_MODER_MODER7_1 |     
                     GPIO_MODER_MODER15_1);    

    // Configure alternate functions for SPI1
    GPIOA->AFR[0] &= ~(GPIO_AFRL_AFRL5 | GPIO_AFRL_AFRL7);  // Clear AF for PA5, PA7
    // AF0 = SPI1 for these pins, so no need to set bits
    GPIOA->AFR[1] &= ~(GPIO_AFRH_AFRH7);      // Clear AF for PA15
    // AF0 = SPI1 for PA15, so no need to set bits

    // Disable SPI before configuring
    SPI1->CR1 &= ~SPI_CR1_SPE;

    // Configure SPI1 CR1
    SPI1->CR1 |= SPI_CR1_MSTR;                // Master mode
    SPI1->CR1 |= (SPI_CR1_BR_2 |              // Baud rate control
                  SPI_CR1_BR_1 |              // BR[2:0] = 111
                  SPI_CR1_BR_0);              // for lowest speed

    // Configure SPI1 CR2
    SPI1->CR2 &= ~SPI_CR2_DS;                 // Clear data size bits
    SPI1->CR2 |= (0x9 << SPI_CR2_DS_Pos);     // Set to 10-bit
    SPI1->CR2 |= (SPI_CR2_SSOE |              // SS output enable
                  SPI_CR2_NSSP);              // NSS pulse enable

    // Enable SPI
    SPI1->CR1 |= SPI_CR1_SPE;
}

void spi_cmd(unsigned int data) {
    while(!(SPI1->SR & SPI_SR_TXE)); 
    SPI1->DR = data;                 // RS=0
}

void spi_data(unsigned int data) {
    while(!(SPI1->SR & SPI_SR_TXE)); 
    SPI1->DR = (data | 0x200);       // Write data RS=1
}

void spi1_init_oled(void) {
    nano_wait(1000000);            
    
    spi_cmd(0x38);                   // Function set: 8-bit interface, 2 lines, 5x7 dots
    spi_cmd(0x08);                   // Display OFF
    spi_cmd(0x01);                   // Clear 
    nano_wait(2000000);             
    spi_cmd(0x06);                   // Entry mode set: increment cursor
    spi_cmd(0x02);                   // Return home
    spi_cmd(0x0C);                   // Display ON, cursor OFF
}

void oled_write_string(const char* str, int line) {
    // Set cursor position based on line
    spi_cmd(line ? 0xC0 : 0x80);     // 0x80 for line 1, 0xC0 for line 2
    
    // Write each character
    while(*str) {
        spi_data(*str++);
    }
}

void update_oled_display(void) {
    // Create strings for display
    char score[16], status[16];
    
    // Format score string
    sprintf(score, "Score: %d", snakeLength - INITIAL_SNAKE_LENGTH);
    
    // Format status based on game state
    switch(gameState) {
        case IDLE:
            sprintf(status, "Press to Start");
            break;
        case RUNNING:
            sprintf(status, "Speed: %d", (INITIAL_SNAKE_SPEED - snakeSpeed) / SPEED_INCREASE);
            break;
        case GAMELOST:
            sprintf(status, "Game Over!");
            break;
        case GAMEWON:
            sprintf(status, "You Won!");
            break;
        default:
            sprintf(status, "Unknown State");
    }
    
    // Write to display only if state or score changed
    if(lastGameState != gameState) {
        oled_write_string(score, 0);
        oled_write_string(status, 1);
        lastGameState = gameState;
    }
}
