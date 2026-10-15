/**
 * @file main.c
 * @author Mohammed Shaheen K S
 * @brief Project 04: Cortex-M0 SysTick Timer Periodic Interrupts for STM32F030C8T6
 * @details Configures the core SysTick timer to generate precise 500ms time-base interrupts 
 *          at 8 MHz HSI clock speed, toggling an LED on PA9 inside the core SysTick_Handler 
 *          ISR without utilizing hardware general-purpose timers or HAL/CMSIS dependencies.
 */

#include <stdint.h>

// ----------------------------------------------------------------------------
// Address Definitions
// ----------------------------------------------------------------------------

// Reset & Clock Control (RCC)
#define RCC_AHBENR       (*(volatile uint32_t *)0x40021014)

// GPIOA Registers
#define GPIOA_MODER      (*(volatile uint32_t *)0x48000000)
#define GPIOA_BSRR       (*(volatile uint32_t *)0x48000018)

// Cortex-M0 SysTick Registers (Base: 0xE000E010)
#define SYST_CSR         (*(volatile uint32_t *)0xE000E010) // Control and Status
#define SYST_RVR         (*(volatile uint32_t *)0xE000E014) // Reload Value
#define SYST_CVR         (*(volatile uint32_t *)0xE000E018) // Current Value

volatile uint8_t led_state = 0;

// ----------------------------------------------------------------------------
// Main Application
// ----------------------------------------------------------------------------

int main(void)
{
    // Step 1: Enable GPIOA Peripheral Clock (Bit 17)
    RCC_AHBENR |= (1 << 17);

    // Step 2: Configure PA9 as General Purpose Output Mode (01)
    GPIOA_MODER &= ~(3UL << (2 * 9)); // Clear bits 18 & 19
    GPIOA_MODER |=  (1UL << (2 * 9)); // Set bit 18 to 1 (Output)

    // Initially turn LED ON (PA9 = 3.3V)
    GPIOA_BSRR = (1 << 9);
    led_state = 1;

    // Step 3: Configure SysTick for 500ms Interrupts
    // Core Clock = 8 MHz -> 8,000,000 cycles/sec.
    // 500ms = 4,000,000 clock cycles.
    SYST_RVR = 4000000 - 1; // Load tick threshold
    SYST_CVR = 0;           // Reset current counter to zero

    // Step 4: Start SysTick Timer
    // Bit 0: Counter Enable | Bit 1: Interrupt Enable | Bit 2: Processor Clock Source
    SYST_CSR = (1 << 0) | (1 << 1) | (1 << 2);

    // Step 5: Enable Global CPU Interrupts
    __asm volatile ("cpsie i");

    while (1)
    {
        // Main CPU loop is 100% free for background tasks!
    }
}

// ----------------------------------------------------------------------------
// SysTick Interrupt Service Routine (Standard Core ISR)
// ----------------------------------------------------------------------------

void SysTick_Handler(void)
{
    // Toggle state flag
    led_state = !led_state;

    // Update PA9 output pin
    if (led_state) {
        GPIOA_BSRR = (1 << 9);         // LED ON  (PA9 set HIGH -> 3.3V)
    } else {
        GPIOA_BSRR = (1 << (9 + 16));  // LED OFF (PA9 set LOW -> 0V)
    }
}