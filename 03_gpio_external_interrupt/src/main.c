/**
 * @file main.c
 * @author Mohammed Shaheen K S
 * @brief Project 03: External Interrupts (EXTI) & EXTI Line 14 ISR for STM32F030C8T6
 * @details Configures a tactile button on PC14 to trigger an external hardware interrupt (EXTI14) 
 *          on the falling edge via SYSCFG, toggling an LED on PA11 inside the shared EXTI4_15 
 *          interrupt service routine without polling or HAL/CMSIS dependencies.
 */

#include <stdint.h>

// 1. Memory Address Definitions
#define RCC_AHBENR       (*(volatile uint32_t *)0x40021014)
#define RCC_APB2ENR      (*(volatile uint32_t *)0x40021018)

// GPIOA (Base: 0x48000000)
#define GPIOA_MODER      (*(volatile uint32_t *)0x48000000)
#define GPIOA_BSRR       (*(volatile uint32_t *)0x48000018)

// GPIOC (Base: 0x48000800)
#define GPIOC_MODER      (*(volatile uint32_t *)0x48000800)
#define GPIOC_PUPDR      (*(volatile uint32_t *)0x4800080C)

// System Configuration Controller (SYSCFG)
#define SYSCFG_EXTICR4   (*(volatile uint32_t *)0x40010014)

// External Interrupt Controller (EXTI)
#define EXTI_IMR         (*(volatile uint32_t *)0x40010400)
#define EXTI_FTSR        (*(volatile uint32_t *)0x4001040C)
#define EXTI_PR          (*(volatile uint32_t *)0x40010414)

// Nested Vectored Interrupt Controller (NVIC)
#define NVIC_ISER        (*(volatile uint32_t *)0xE000E100)

volatile uint8_t led_state = 0; // 0 = OFF, 1 = ON

void delay(volatile uint32_t count) {
    while (count--) {
        __asm("nop");
    }
}

int main(void)
{
    // Step 1: Enable Clocks for GPIOA (Bit 17), GPIOC (Bit 19), and SYSCFG (Bit 0)
    RCC_AHBENR  |= (1 << 17) | (1 << 19);

    for (volatile int i = 0; i < 1000; i++); // Settling delay

    // Step 2: Configure PA11 as Output Mode (01)
    GPIOA_MODER &= ~(3UL << (2 * 11));
    GPIOA_MODER |=  (1UL << (2 * 11));

    // Turn LED OFF initially (PA11 HIGH)
    GPIOA_BSRR = (1 << 11);

    // Step 3: Configure PC14 as Input Mode (00) with Internal Pull-Up (01)
    GPIOC_MODER &= ~(3UL << (2 * 14));
    GPIOC_PUPDR &= ~(3UL << (2 * 14));
    GPIOC_PUPDR |=  (1UL << (2 * 14));

    // Step 4: Route PC14 to EXTI14 using SYSCFG_EXTICR4
    // Bits 11:8 control EXTI14 selection (0x2 selects Port C)
    SYSCFG_EXTICR4 &= ~(0xF << 8); // Clear bits 8, 9, 10, 11
    SYSCFG_EXTICR4 |=  (0x2 << 8); // Set to 0010 (Port C)

    // Step 5: Configure EXTI Line 14 Mask and Falling Edge Trigger
    EXTI_IMR  |= (1 << 14); // Unmask EXTI Line 14
    EXTI_FTSR |= (1 << 14); // Trigger on Falling Edge (HIGH -> LOW on button press)

    // Step 6: Enable EXTI4_15 Interrupt in NVIC
    // On STM32F030, EXTI lines 4 through 15 share NVIC Position 7
    NVIC_ISER = (1 << 7);

    while (1)
    {
        // CPU main loop remains free!
    }
}

// Step 7: Interrupt Handler Function
// Function name matches the vector table in startup_stm32f030x8.s
void EXTI4_15_IRQHandler(void)
{
    // Check if EXTI Line 14 generated the interrupt request
    if (EXTI_PR & (1 << 14))
    {
        // Software Debounce
        delay(50000);

        // Toggle State
        led_state = !led_state;

        if (led_state) {
            GPIOA_BSRR = (1 << (11 + 16)); // LED ON (Reset PA11 LOW)
        } else {
            GPIOA_BSRR = (1 << 11);         // LED OFF (Set PA11 HIGH)
        }

        // CLEAR Pending Interrupt Flag by writing a 1
        EXTI_PR = (1 << 14);
    }
}