/**
 * @file main.c
 * @author Mohammed Shaheen K S
 * @brief Project 08: Master-Slave Timer Synchronization (TIM1 & TIM3) for STM32F030C8T6
 * @details Configures advanced-control timer TIM1 as a master generating a 1 Hz PWM signal 
 *          and broadcasting trigger pulses (TRGO) via internal trigger ITR0 to reset the 
 *          slave general-purpose timer TIM3, synchronizing cascaded hardware outputs on pins 
 *          PA8 and PA6 without CPU overhead or HAL/CMSIS dependencies.
 */

#include <stdint.h>

// ============================================================================
// 1. REGISTER DEFINITIONS & MEMORY MAP (STM32F030 x8 Value Line)
// ============================================================================
#define RCC_AHBENR   (*(volatile uint32_t *)0x40021014)
#define RCC_APB2ENR  (*(volatile uint32_t *)0x40021018) // TIM1 Clock
#define RCC_APB1ENR  (*(volatile uint32_t *)0x4002101C) // TIM3 Clock

#define GPIOA_MODER  (*(volatile uint32_t *)0x48000000)
#define GPIOA_AFRL   (*(volatile uint32_t *)0x48000020) // AF Pin 0 to 7
#define GPIOA_AFRH   (*(volatile uint32_t *)0x48000024) // AF Pin 8 to 15

typedef struct {
    volatile uint32_t CR1;   // 0x00 Control Register 1
    volatile uint32_t CR2;   // 0x04 Control Register 2
    volatile uint32_t SMCR;  // 0x08 Slave Mode Control Register
    volatile uint32_t DIER;  // 0x0C DMA/Interrupt Enable Register
    volatile uint32_t SR;    // 0x10 Status Register
    volatile uint32_t EGR;   // 0x14 Event Generation Register
    volatile uint32_t CCMR1; // 0x18 Capture/Compare Mode Register 1
    volatile uint32_t CCMR2; // 0x1C Capture/Compare Mode Register 2
    volatile uint32_t CCER;  // 0x20 Capture/Compare Enable Register
    volatile uint32_t CNT;   // 0x24 Counter Register
    volatile uint32_t PSC;   // 0x28 Prescaler Register
    volatile uint32_t ARR;   // 0x2C Auto-Reload Register
    volatile uint32_t RCR;   // 0x30 Repetition Counter Register
    volatile uint32_t CCR1;  // 0x34 Capture/Compare Register 1
    volatile uint32_t CCR2;  // 0x38 Capture/Compare Register 2
    volatile uint32_t CCR3;  // 0x3C Capture/Compare Register 3
    volatile uint32_t CCR4;  // 0x40 Capture/Compare Register 4
    volatile uint32_t BDTR;  // 0x44 Break and Dead-Time Register (TIM1 only)
} TIM_TypeDef;

#define TIM1 ((TIM_TypeDef *) 0x40012C00) // Master Timer (Advanced)
#define TIM3 ((TIM_TypeDef *) 0x40000400) // Slave Timer (General Purpose)

// ============================================================================
// 2. MAIN APPLICATION
// ============================================================================
int main(void)
{
    // ------------------------------------------------------------------------
    // Step 1: Enable Peripheral Clocks
    // ------------------------------------------------------------------------
    RCC_AHBENR  |= (1UL << 17);       // Enable GPIOA Clock
    RCC_APB2ENR |= (1UL << 11);       // Enable TIM1 Clock (Master)
    RCC_APB1ENR |= (1UL << 1);        // Enable TIM3 Clock (Slave)

    // ------------------------------------------------------------------------
    // Step 2: Configure Pins PA8 (TIM1_CH1) and PA6 (TIM3_CH1)
    // ------------------------------------------------------------------------
    // PA8 -> Alternate Function Mode (AF2 = TIM1_CH1)
    GPIOA_MODER &= ~(3UL << (2 * 8));
    GPIOA_MODER |=  (2UL << (2 * 8));   // Mode 10: Alternate Function
    GPIOA_AFRH  &= ~(0xFUL << (4 * 0)); // Bit field for Pin 8 in AFRH
    GPIOA_AFRH  |=  (0x2UL << (4 * 0)); // AF2 = TIM1_CH1

    // PA6 -> Alternate Function Mode (AF1 = TIM3_CH1)
    GPIOA_MODER &= ~(3UL << (2 * 6));
    GPIOA_MODER |=  (2UL << (2 * 6));   // Mode 10: Alternate Function
    GPIOA_AFRL  &= ~(0xFUL << (4 * 6)); // Bit field for Pin 6 in AFRL
    GPIOA_AFRL  |=  (0x1UL << (4 * 6)); // AF1 = TIM3_CH1

    // ------------------------------------------------------------------------
    // Step 3: Configure Master Timer (TIM1) - 1 Hz (1 Second Period)
    // ------------------------------------------------------------------------
    TIM1->PSC = 8000 - 1;               // 1 kHz tick rate (assumes 8 MHz default HSI)
    TIM1->ARR = 1000 - 1;               // 1000 ms (1 Second Period)
    TIM1->CCR1 = 500;                   // 500 ms ON time (50% Duty Cycle)

    // Configure CH1 in PWM Mode 1 (OC1M = 110) with Preload Enabled
    TIM1->CCMR1 &= ~(3UL << 0);         // CC1S = Output
    TIM1->CCMR1 &= ~(7UL << 4);
    TIM1->CCMR1 |=  (6UL << 4);         // PWM Mode 1
    TIM1->CCMR1 |=  (1UL << 3);         // Preload Enable

    TIM1->CCER &= ~(1UL << 1);          // Active High Polarity
    TIM1->CCER |=  (1UL << 0);          // Output Enable (CC1E = 1)

    // Advanced Timers (TIM1) require Main Output Enable (MOE) bit in BDTR
    TIM1->BDTR |=  (1UL << 15);         // MOE = 1 (Main Output Enable)

    // Master Mode Selection: Output TRGO pulse on Update Event (MMS = 010)
    TIM1->CR2 &= ~(7UL << 4);
    TIM1->CR2 |=  (2UL << 4);

    // ------------------------------------------------------------------------
    // Step 4: Configure Slave Timer (TIM3) - Reset Mode
    // ------------------------------------------------------------------------
    TIM3->PSC = 8000 - 1;               // 1 kHz tick rate
    TIM3->ARR = 1000 - 1;               // 1000 ms Period
    TIM3->CCR1 = 100;                   // 100 ms ON time (Short Flash)

    // Configure CH1 in PWM Mode 1 (OC1M = 110) with Preload Enabled
    TIM3->CCMR1 &= ~(3UL << 0);         // CC1S = Output
    TIM3->CCMR1 &= ~(7UL << 4);
    TIM3->CCMR1 |=  (6UL << 4);         // PWM Mode 1
    TIM3->CCMR1 |=  (1UL << 3);         // Preload Enable

    TIM3->CCER &= ~(1UL << 1);          // Active High Polarity
    TIM3->CCER |=  (1UL << 0);          // Output Enable (CC1E = 1)

    // Slave Trigger Source: ITR0 (TIM1 TRGO) -> TS = 000
    TIM3->SMCR &= ~(7UL << 4);          // TS = 000 (ITR0 connected to TIM1)

    // Slave Mode: Reset Mode (SMS = 100)
    TIM3->SMCR &= ~(7UL << 0);
    TIM3->SMCR |=  (4UL << 0);          // Resets TIM3->CNT to 0 on TIM1 TRGO edge

    // ------------------------------------------------------------------------
    // Step 5: Force Initial Shadow Load & Start Timers
    // ------------------------------------------------------------------------
    TIM1->EGR |= (1UL << 0);            // Load shadow registers for TIM1
    TIM3->EGR |= (1UL << 0);            // Load shadow registers for TIM3

    TIM1->SR  &= ~(1UL << 0);           // Clear pending update flags
    TIM3->SR  &= ~(1UL << 0);

    TIM3->CR1 |= (1UL << 0);            // Enable Slave Counter (TIM3 CEN = 1)
    TIM1->CR1 |= (1UL << 0);            // Enable Master Counter (TIM1 CEN = 1)

    while (1)
    {
        // CPU remains idle; hardware outputs clean cascaded PWM
    }
}