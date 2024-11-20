#include "superSnake.h"
#include "fifo.h"
#include "tty.h"

void init_spi1() {
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;

GPIOA->MODER = (GPIOA->MODER & ~((3 << (5*2)) | (3 << (6*2)) | (3 << (7*2)) | (3 << (15*2)))) 
               | (2 << (5*2)) | (2 << (6*2)) | (2 << (7*2)) | (2 << (15*2));

GPIOA->AFR[0] = (GPIOA->AFR[0] & ~((0xF << (5*4)) | (0xF << (6*4)) | (0xF << (7*4)))) 
                | (0 << (5*4)) | (0 << (6*4)) | (0 << (7*4));
GPIOA->AFR[1] = (GPIOA->AFR[1] & ~(0xF << ((15-8)*4))) | (0 << ((15-8)*4));

SPI1->CR1 = (SPI1->CR1 & ~(SPI_CR1_SPE | SPI_CR1_BR)) | SPI_CR1_MSTR | SPI_CR1_BR_2 | SPI_CR1_BR_1 | SPI_CR1_BR_0;
SPI1->CR2 = (SPI1->CR2 & ~(SPI_CR2_DS)) | SPI_CR2_DS_0 | SPI_CR2_DS_3 | SPI_CR2_SSOE | SPI_CR2_NSSP | SPI_CR2_TXDMAEN;

SPI1->CR1 |= SPI_CR1_SPE;

}

uint16_t display[34] = {
        0x002, // Command to set the cursor at the first position line 1
        0x200+'E', 0x200+'C', 0x200+'E', 0x200+'3', 0x200+'6', + 0x200+'2', 0x200+' ', 0x200+'i',
        0x200+'s', 0x200+' ', 0x200+'t', 0x200+'h', + 0x200+'e', 0x200+' ', 0x200+' ', 0x200+' ',
        0x0c0, // Command to set the cursor at the first position line 2
        0x200+'c', 0x200+'l', 0x200+'a', 0x200+'s', 0x200+'s', + 0x200+' ', 0x200+'f', 0x200+'o',
        0x200+'r', 0x200+' ', 0x200+'y', 0x200+'o', + 0x200+'u', 0x200+'!', 0x200+' ', 0x200+' ',
};


void spi1_init_oled() {
    nano_wait(1000000);
    spi_cmd(0x38);
    spi_cmd(0x08);
    //check again
    spi_cmd(0x01);
    nano_wait(2000000);
    spi_cmd(0x06);
    spi_cmd(0x02);
    spi_cmd(0x0c);
}

void spi1_setup_dma(void) {
     RCC->AHBENR |= RCC_AHBENR_DMA1EN;
    DMA1_Channel3->CCR = DMA_CCR_DIR | DMA_CCR_MINC | DMA_CCR_MSIZE_0 | DMA_CCR_PSIZE_0 | DMA_CCR_CIRC;
    DMA1_Channel3->CPAR = (uint32_t)&SPI1->DR;
    DMA1_Channel3->CMAR = (uint32_t)display;
    DMA1_Channel3->CNDTR = 34;
    DMA1_Channel3->CCR &= ~DMA_CCR_EN;
    SPI1->CR2 |= SPI_CR2_TXDMAEN;
}

void spi1_enable_dma(void) {
    DMA1_Channel3->CCR |= DMA_CCR_EN;  
}


void spi1_dma_display1(const char *str)
{
    for(int i=0; i<16; i++) {
        if (str[i])
            display[i+1] = 0x200 + str[i];
        else {
            // End of string.  Pad with spaces.
            for(int j=i; j<16; j++)
                display[j+1] = 0x200 + ' ';
            break;
        }
    }
}

void spi1_dma_display2(const char *str)
{
    for(int i=0; i<16; i++) {
        if (str[i])
            display[i+18] = 0x200 + str[i];
        else {
            // End of string.  Pad with spaces.
            for(int j=i; j<16; j++)
                display[j+18] = 0x200 + ' ';
            break;
        }
    }
}





// void init_spi1(void) {
//     RCC->AHBENR |= RCC_AHBENR_GPIOAEN;        // Enable GPIOA 
//     RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;       // Enable SPI1 

//     // pins PA5(SCK), PA7(MOSI), PA15(NSS)
//     //spi1
//     //set using alternate func
//     GPIOA->MODER &= ~(GPIO_MODER_MODER5 | GPIO_MODER_MODER7 | GPIO_MODER_MODER15);
//     GPIOA->MODER |= (GPIO_MODER_MODER5_1 |   
//                      GPIO_MODER_MODER7_1 |     
//                      GPIO_MODER_MODER15_1);    

//     // alternate functions for SPI1
//     GPIOA->AFR[0] &= ~(GPIO_AFRL_AFRL5 | GPIO_AFRL_AFRL7); 
//     // AF0 = SPI1 for these pins, so no need to set bits
//     GPIOA->AFR[1] &= ~(GPIO_AFRH_AFRH7);      // Clear AF for PA15
//     // AF0 = SPI1 for PA15, so no need to set bits

//     // Disable SPI 
//     SPI1->CR1 &= ~SPI_CR1_SPE;

//     //SPI1 CR1
//     SPI1->CR1 |= SPI_CR1_MSTR;                // Master mode
//     SPI1->CR1 |= (SPI_CR1_BR_2 |              // Baud rate control
//                   SPI_CR1_BR_1 |              // BR[2:0] = 111
//                   SPI_CR1_BR_0);              // for lowest speed

//     // Configure SPI1 CR2
//     SPI1->CR2 &= ~SPI_CR2_DS;                 // Clear data size bits
//     SPI1->CR2 |= (0x9 << SPI_CR2_DS_Pos);     // Set to 10-bit
//     SPI1->CR2 |= (SPI_CR2_SSOE |              // SS output enable
//                   SPI_CR2_NSSP);              // NSS pulse enable

//     // Enable SPI
//     SPI1->CR1 |= SPI_CR1_SPE;
// }

// void spi_cmd(unsigned int data) {
//     while(!(SPI1->SR & SPI_SR_TXE)); 
//     SPI1->DR = data;                 // RS=0
// }

// void spi_data(unsigned int data) {
//     while(!(SPI1->SR & SPI_SR_TXE)); 
//     SPI1->DR = (data | 0x200);       // Write data RS=1
// }

// void spi1_init_oled(void) {
//     nano_wait(1000000);            
    
//     spi_cmd(0x38);                   // Function set: 8-bit interface, 2 lines, 5x7 dots
//     spi_cmd(0x08);                   // Display OFF
//     spi_cmd(0x01);                   // Clear 
//     nano_wait(2000000);             
//     spi_cmd(0x06);                   // Entry mode set: increment cursor
//     spi_cmd(0x02);                   // Return home
//     spi_cmd(0x0C);                   // Display ON, cursor OFF
// }

// void oled_write_string(const char* str, int line) {
//     if (line == 1) {
//         spi_cmd(0xC0);    //second line
//     } else {
//         spi_cmd(0x80);    //irst line
//     }
    
//     while(*str) {
//         spi_data(*str++);
//     }
// }

// void oled_write_string(const char* str, int line) {
//     // Set cursor position based on line
//     spi_cmd(line ? 0xC0 : 0x80);     // 0x80 for line 1, 0xC0 for line 2
    
//     // Write each character
//     while(*str) {
//         spi_data(*str++);
//     }
// }

// void update_oled_display(void) {
//     // Create strings for display
//     char score[16], status[16];
    
//     sprintf(score, "Score: %d", snakeLength - INITIAL_SNAKE_LENGTH);
    
//     switch(gameState) {
//         case IDLE:
//             sprintf(status, "Press to Start");
//             break;
//         case RUNNING:
//             sprintf(status, "Speed: %d", (INITIAL_SNAKE_SPEED - snakeSpeed) / SPEED_INCREASE);
//             break;
//         case GAMELOST:
//             sprintf(status, "Game Over!");
//             break;
//         case GAMEWON:
//             sprintf(status, "You Won!");
//             break;
//         default:
//             sprintf(status, "Unknown State");
//     }
    
//     // Write to display only if state or score changed
//     if(lastGameState != gameState) {
//         oled_write_string(score, 0);
//         oled_write_string(status, 1);
//         lastGameState = gameState;
//     }
// }
