/**
 * @file main.c
 * @author Mohammed Shaheen K S
 * @brief Project 01: Bare-Metal GPIO LED Blink for STM32F030C8T6
 * @details Toggles an LED connected to PA9 using direct register-level 
 *          manipulation (RCC, MODER, and BSRR) without HAL or CMSIS.
 */

#include <stdint.h>

// Hardware Register Addresses for STM32F030 (Port A)
#define RCC_AHBENR   (*(volatile uint32_t *)0x40021014)
#define GPIOA_MODER  (*(volatile uint32_t *)0x48000000)
#define GPIOA_BSRR   (*(volatile uint32_t *)0x48000018)

void delay(volatile uint32_t count) {
    while (count--) {
        __asm("nop");
    }
}

int main(void)
{
    // 1. Enable GPIOA Clock (Bit 17)
    RCC_AHBENR |= (1 << 17);
    for (volatile int i = 0; i < 1000; i++); // Settling delay

    // 2. Configure PA9 as Output Mode (01)
    // Bits 18 & 19 control Pin 9
    GPIOA_MODER &= ~(3UL << (2 * 9)); // Clear bits 18 & 19
    GPIOA_MODER |=  (1UL << (2 * 9)); // Set bit 18 to 1 (Output)

    while (1)
    {
        // 3. LED ON (PA9 LOW -> Reset bit 9 via Bit 25)
        GPIOA_BSRR = (1 << (9 + 16));
        delay(300000);

        // 4. LED OFF (PA9 HIGH -> Set bit 9)
        GPIOA_BSRR = (1 << 9);
        delay(300000);
    }
}