/**
 * @file main.c
 * @author Mohammed Shaheen K S
 * @brief Project 05: General-Purpose Timer (TIM3) Hardware PWM & Breathing LED for STM32F030C8T6
 * @details Configures general-purpose timer TIM3 Channel 1 on pin PA6 to generate a 1 kHz 
 *          hardware PWM signal using the 8 MHz HSI clock, dynamically sweeping the compare 
 *          register (CCR1) to create a smooth fading (breathing) LED effect without HAL or CMSIS dependencies.
 */

#include <stdint.h>

// ============================================================================
// 1. REGISTER DEFINITIONS
// ============================================================================
#define RCC_AHBENR       (*(volatile uint32_t *)0x40021014)
#define RCC_APB1ENR      (*(volatile uint32_t *)0x4002101C)

#define GPIOA_MODER      (*(volatile uint32_t *)0x48000000)
#define GPIOA_AFRL       (*(volatile uint32_t *)0x48000020)

typedef struct {
    volatile uint32_t CR1;   // 0x00: Control Register 1
    volatile uint32_t CR2;   // 0x04: Control Register 2
    volatile uint32_t SMCR;  // 0x08: Slave Mode Control Register
    volatile uint32_t DIER;  // 0x0C: DMA/Interrupt Enable Register
    volatile uint32_t SR;    // 0x10: Status Register
    volatile uint32_t EGR;   // 0x14: Event Generation Register
    volatile uint32_t CCMR1; // 0x18: Capture/Compare Mode Register 1
    volatile uint32_t CCMR2; // 0x1C: Capture/Compare Mode Register 2
    volatile uint32_t CCER;  // 0x20: Capture/Compare Enable Register
    volatile uint32_t CNT;   // 0x24: Counter Register
    volatile uint32_t PSC;   // 0x28: Prescaler Register
    volatile uint32_t ARR;   // 0x2C: Auto-Reload Register
    volatile uint32_t RESERVED[1];
    volatile uint32_t CCR1;  // 0x34: Capture/Compare Register 1
} TIM_TypeDef;

#define TIM3 ((TIM_TypeDef *) 0x40000400)

// ============================================================================
// 2. MAIN APPLICATION
// ============================================================================

int main(void)
{
    // Step 1: Enable Clocks for GPIOA (Bit 17) and TIM3 (Bit 1 on APB1)
    RCC_AHBENR  |= (1 << 17);
    RCC_APB1ENR |= (1 << 1);

    // Dummy read to ensure clock bus pipeline stabilizes
    volatile uint32_t dummy = RCC_APB1ENR;
    (void)dummy;

    // Step 2: Configure Pin PA6 to Alternate Function Mode (AF1 = TIM3_CH1)
    // Clear mode bits for PA6 (bits 12 & 13) and set to Alternate Function Mode (10)
    GPIOA_MODER &= ~(3UL << (2 * 6));
    GPIOA_MODER |=  (2UL << (2 * 6));

    // Map PA6 to Alternate Function 1 (AF1 / TIM3_CH1) in AFRL
    GPIOA_AFRL &= ~(0xFUL << (4 * 6)); // Clear bits 24-27
    GPIOA_AFRL |=  (0x1UL << (4 * 6)); // AF1 = 0001

    // Step 3: Configure TIM3 Timebase for 1 kHz PWM Frequency (at 8 MHz HSI Clock)
    // PSC = 8 - 1  --> 8 MHz / 8 = 1 MHz counter tick clock (1 us per tick)
    TIM3->PSC = 8 - 1;

    // ARR = 1000 - 1 --> 1000 ticks * 1 us = 1000 us (1 ms period = 1 kHz frequency)
    TIM3->ARR = 1000 - 1;

    // Step 4: Configure TIM3 Channel 1 for PWM Mode 1
    // OC1M = 110 (PWM Mode 1: Active while CNT < CCR1, inactive otherwise)
    // OC1PE = 1  (Enable Preload register for CCR1)
    TIM3->CCMR1 &= ~(7UL << 4);
    TIM3->CCMR1 |=  (6UL << 4) | (1UL << 3);

    // Set initial Duty Cycle = 25% (250 / 1000)
    TIM3->CCR1 = 250;

    // Step 5: Enable Channel 1 Output Signal (CC1E bit)
    TIM3->CCER |= (1 << 0);

    // Step 6: Force Update Event to load PSC, ARR, and CCR registers
    TIM3->EGR = (1 << 0);

    // Step 7: Start Timer Counter (CEN bit)
    TIM3->CR1 |= (1 << 0);

    // Simple breathing LED loop adjusting CCR1 (Duty Cycle) dynamically
    uint16_t duty = 0;
    int8_t step = 5;

    while (1)
    {
        // Update duty cycle (0 to 1000)
        TIM3->CCR1 = duty;

        duty += step;
        if (duty >= 1000 || duty <= 0) {
            step = -step; // Reverse dimming direction
        }

        // Blocking delay for smooth fading effect (~5 ms delay)
        for (volatile uint32_t i = 0; i < 4000; i++);
    }
}