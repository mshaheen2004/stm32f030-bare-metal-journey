/**
 * @file main.c
 * @author Mohammed Shaheen K S
 * @brief Project 06: Advanced Control Timer (TIM1) Complementary PWM & Dead-Time Insertion for STM32F030C8T6
 * @details Configures advanced-control timer TIM1 to generate a 1 kHz complementary PWM signal pair 
 *          on pins PA8 (CH1) and PA7 (CH1N) with hardware dead-time insertion via the BDTR register 
 *          and Main Output Enable (MOE), driving complementary fading LEDs without HAL or CMSIS dependencies.
 */

#include <stdint.h>

// ============================================================================
// 1. REGISTER DEFINITIONS
// ============================================================================
#define RCC_AHBENR       (*(volatile uint32_t *)0x40021014)
#define RCC_APB2ENR      (*(volatile uint32_t *)0x40021018)

#define GPIOA_MODER      (*(volatile uint32_t *)0x48000000)
#define GPIOA_AFRL       (*(volatile uint32_t *)0x48000020)
#define GPIOA_AFRH       (*(volatile uint32_t *)0x48000024)

typedef struct {
    volatile uint32_t CR1;       // 0x00: Control Register 1
    volatile uint32_t CR2;       // 0x04: Control Register 2
    volatile uint32_t SMCR;      // 0x08: Slave Mode Control Register
    volatile uint32_t DIER;      // 0x0C: DMA/Interrupt Enable Register
    volatile uint32_t SR;        // 0x10: Status Register
    volatile uint32_t EGR;       // 0x14: Event Generation Register
    volatile uint32_t CCMR1;     // 0x18: Capture/Compare Mode Register 1
    volatile uint32_t CCMR2;     // 0x1C: Capture/Compare Mode Register 2
    volatile uint32_t CCER;      // 0x20: Capture/Compare Enable Register
    volatile uint32_t CNT;       // 0x24: Counter Register
    volatile uint32_t PSC;       // 0x28: Prescaler Register
    volatile uint32_t ARR;       // 0x2C: Auto-Reload Register
    volatile uint32_t RCR;       // 0x30: Repetition Counter Register
    volatile uint32_t CCR1;      // 0x34: Capture/Compare Register 1
    volatile uint32_t CCR2;      // 0x38: Capture/Compare Register 2
    volatile uint32_t CCR3;      // 0x3C: Capture/Compare Register 3
    volatile uint32_t CCR4;      // 0x40: Capture/Compare Register 4
    volatile uint32_t BDTR;      // 0x44: Break and Dead-Time Register
} TIM_TypeDef;

#define TIM1 ((TIM_TypeDef *) 0x40012C00) // Base address for TIM1 (APB2)

// ============================================================================
// 2. MAIN APPLICATION
// ============================================================================

int main(void)
{
    // Step 1: Enable Clocks for GPIOA (Bit 17) and TIM1 (Bit 11 on APB2)
    RCC_AHBENR  |= (1 << 17);
    RCC_APB2ENR |= (1 << 11);

    // Dummy read to stabilize clock pipeline
    volatile uint32_t dummy = RCC_APB2ENR;
    (void)dummy;

    // Step 2: Configure Pins PA8 (TIM1_CH1) & PA7 (TIM1_CH1N) to AF Mode
    // Clear mode bits for PA8 & PA7, then set to Alternate Function Mode (10)
    GPIOA_MODER &= ~((3UL << (2 * 8)) | (3UL << (2 * 7)));
    GPIOA_MODER |=  ((2UL << (2 * 8)) | (2UL << (2 * 7)));

    // Map PA8 to AF2 (TIM1_CH1) in AFRH register (Bits [3:0])
    GPIOA_AFRH &= ~(0xFUL << (4 * 0));
    GPIOA_AFRH |=  (0x2UL << (4 * 0)); // AF2 = 0010

    // Map PA7 to AF2 (TIM1_CH1N) in AFRL register (Bits [31:28])
    GPIOA_AFRL &= ~(0xFUL << (4 * 7));
    GPIOA_AFRL |=  (0x2UL << (4 * 7)); // AF2 = 0010

    // Step 3: Timebase Setup (1 kHz PWM for smooth visual fading at 8 MHz HSI)
    TIM1->PSC = 8 - 1;          // 8 MHz / 8 = 1 MHz counter tick (1 us per tick)
    TIM1->ARR = 1000 - 1;       // 1000 ticks * 1 us = 1 ms period (1 kHz frequency)

    // Step 4: Configure Channel 1 for PWM Mode 1 & enable CCR1 Preload
    TIM1->CCMR1 &= ~(7UL << 4);
    TIM1->CCMR1 |=  (6UL << 4) | (1UL << 3); // OC1M = 110 (PWM Mode 1), OC1PE = 1

    // Step 5: Configure Dead-Time and Enable Main Output (BDTR Register)
    // DTG[7:0] = 50 ticks -> 50 us Dead Time inserted between transitions
    TIM1->BDTR &= ~(0xFFUL);
    TIM1->BDTR |= 50;

    // MOE Bit (Bit 15) = 1 -> Main Output Enable (Required for TIM1 outputs)
    TIM1->BDTR |= (1 << 15);

    // Step 6: Enable Channel 1 Output (CC1E) AND Complementary Output (CC1NE)
    TIM1->CCER |= (1 << 0); // Bit 0: CC1E  (Enable PA8 / CH1 -> LED 1)
    TIM1->CCER |= (1 << 2); // Bit 2: CC1NE (Enable PA7 / CH1N -> LED 2)

    // Step 7: Load settings and start counter
    TIM1->EGR |= (1 << 0);  // Generate update event to shadow PSC/ARR
    TIM1->CR1 |= (1 << 0);  // Start TIM1 counter (CEN)

    // ========================================================================
    // DYNAMIC 2-LED COMPLEMENTARY FADING LOOP
    // ========================================================================
    uint16_t duty = 0;
    int16_t step = 5;

    while (1)
    {
        // Update TIM1 Channel 1 Duty Cycle (0 to 1000)
        TIM1->CCR1 = duty;

        duty += step;

        // Reverse direction at boundary limits
        if (duty >= 1000 || duty <= 0) {
            step = -step;
        }

        // Blocking delay (~5 ms) for smooth visible transition
        for (volatile uint32_t i = 0; i < 4000; i++);
    }
}