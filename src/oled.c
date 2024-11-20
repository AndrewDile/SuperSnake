#include "stm32f0xx.h"

void nano_wait(unsigned int n) {
    asm(    "        mov r0,%0\n"
            "repeat: sub r0,#83\n"
            "        bgt repeat\n" 
            : : "r"(n) : "r0", "cc");
}

void init_oled_gpio(void) {
    // Enable GPIOA and GPIOB clocks
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN | RCC_AHBENR_GPIOBEN;
    
    // PA4=RS(4), PA5=E(6) as outputs
    GPIOA->MODER |= (1<<8) | (1<<10);
    
    // PB0-7 as outputs for DB0-DB7(7-14)
    GPIOB->MODER |= 0x5555;
}

void write_cmd(uint8_t cmd) {
    GPIOA->ODR &= ~(1<<4);    // RS = 0 for command
    GPIOB->ODR = cmd;         // Put command on data bus
    
    // Generate E pulse
    GPIOA->ODR |= (1<<5);     // E = 1
    nano_wait(500);           // Wait >450ns
    GPIOA->ODR &= ~(1<<5);    // E = 0
    nano_wait(600000);        // Wait >600us for command completion
}

void write_data(uint8_t data) {
    GPIOA->ODR |= (1<<4);     // RS = 1 for data
    GPIOB->ODR = data;        // Put data on data bus
    
    // Generate E pulse
    GPIOA->ODR |= (1<<5);     // E = 1
    nano_wait(500);           // Wait >450ns
    GPIOA->ODR &= ~(1<<5);    // E = 0
    nano_wait(600000);        // Wait >600us for data completion
}

void init_oled(void) {
    nano_wait(50000000);      // Wait >40ms after power up
    
    write_cmd(0x38);          // Function Set: 8-bit, 2-line, 5x8 dots
    nano_wait(50000);
    
    write_cmd(0x08);          // Display off
    nano_wait(50000);
    
    write_cmd(0x01);          // Clear display
    nano_wait(2000000);       // Clear needs 2ms
    
    write_cmd(0x06);          // Entry mode set: increment, no shift
    nano_wait(50000);
    
    write_cmd(0x0C);          // Display ON, no cursor
    nano_wait(50000);
}

void oled_write_string(const char* str, int line) {
    write_cmd(line ? 0xC0 : 0x80);  // Set line 1 or 2 position
    while(*str) write_data(*str++);
}

void oled_write_two_lines(const char* line1, const char* line2) {
    oled_write_string(line1, 0);
    oled_write_string(line2, 1);
}