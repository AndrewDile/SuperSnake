#include "stm32f0xx.h"

void nano_wait(unsigned int n) {
    asm(    "        mov r0,%0\n"
            "repeat: sub r0,#83\n"
            "        bgt repeat\n" 
            : : "r"(n) : "r0", "cc");
}

void init_oled_gpio(void) {
    RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
    
    // Configure PC0-PC10 as outputs
    // Set pairs of bits to 01 for output mode
    GPIOC->MODER |= 0x155555;    // Sets PC0-PC10 to output mode
}

void write_cmd(uint8_t cmd) {
    // RS = 0 (PC0) for command
    GPIOC->ODR &= ~(1<<0);
    // RW = 0 (PC1) for write
    GPIOC->ODR &= ~(1<<1);
    
    // Put command on data bus (PC3-PC10)
    GPIOC->ODR = (GPIOC->ODR & ~(0xFF << 3)) | (cmd << 3);
    
    // Generate E pulse (PC2)
    GPIOC->ODR |= (1<<2);
    nano_wait(500);
    GPIOC->ODR &= ~(1<<2);
    nano_wait(100000);
}

void write_data(uint8_t data) {
    // RS = 1 for data
    GPIOC->ODR |= (1<<0);
    // RW = 0 for write
    GPIOC->ODR &= ~(1<<1);
    
    // Put data on data bus (PC3-PC10)
    GPIOC->ODR = (GPIOC->ODR & ~(0xFF << 3)) | (data << 3);
    
    // Generate E pulse
    GPIOC->ODR |= (1<<2);
    nano_wait(500);
    GPIOC->ODR &= ~(1<<2);
    nano_wait(100000);
}

void init_oled(void) {
    nano_wait(50000000);      // Wait >40ms after power up
    
    write_cmd(0x38);          // Function Set: 8-bit, 2-line, 5x8 dots
    write_cmd(0x08);          // Display off
    write_cmd(0x01);          // Clear display
    nano_wait(2000000);       // Clear needs 2ms
    write_cmd(0x06);          // Entry mode set: increment, no shift
    write_cmd(0x0C);          // Display ON, no cursor
}

void oled_write_string(const char* str, int line) {
    // Set cursor to beginning of specified line
    write_cmd(line ? 0xC0 : 0x80);
    
    // Write each character
    while(*str) write_data(*str++);
}

void oled_write_two_lines(const char* line1, const char* line2) {
    oled_write_string(line1, 0);
    oled_write_string(line2, 1);
}