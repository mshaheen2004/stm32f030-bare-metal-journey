/**
 * @file main.c
 * @author Mohammed Shaheen K S
 * @brief Project 07: General-Purpose Timer Input Capture Mode & Pulse-Width Measurement for STM32F030C8T6
 * @details Configures a general-purpose timer in counter mode with input pin polling (PA0) 
 *          to measure tactile button press durations in microseconds with 1 MHz tick resolution, 
 *          controlling an onboard LED status indicator on PA5 without HAL or CMSIS dependencies.
 */

#include <stdint.h>

#define RCC_AHBENR   (*(volatile uint32_t *)0x40021014)
#define RCC_APB1ENR  (*(volatile uint32_t *)0x4002101C)

#define GPIOA_MODER  (*(volatile uint32_t *)0x48000000)
#define GPIOA_PUPDR  (*(volatile uint32_t *)0x4800000C)
#define GPIOA_IDR    (*(volatile uint32_t *)0x48000010)
#define GPIOA_ODR    (*(volatile uint32_t *)0x48000014)
#define GPIOA_AFRL   (*(volatile uint32_t *)0x48000020)

typedef struct {
    volatile uint32_t CR1;
    volatile uint32_t CR2;
    volatile uint32_t SMCR;
    volatile uint32_t DIER;
    volatile uint32_t SR;
    volatile uint32_t EGR;
    volatile uint32_t CCMR1;
    volatile uint32_t CCMR2;
    volatile uint32_t CCER;
    volatile uint32_t CNT;
    volatile uint32_t PSC;
    volatile uint32_t ARR;
    volatile uint32_t RCR;
    volatile uint32_t CCR1;
    volatile uint32_t CCR2;
} TIM_TypeDef;

#define TIM2 ((TIM_TypeDef *) 0x40000000)

volatile uint32_t press_duration_us = 0;

int main(void)
{
    // 1. Enable GPIOA and TIM2 clocks
    RCC_AHBENR  |= (1 << 17);
    RCC_APB1ENR |= (1 << 0);

    // 2. PA0 as Alternate Function for TIM2_CH1
    GPIOA_MODER &= ~(3UL << (2 * 0));
    GPIOA_MODER |=  (2UL << (2 * 0)); // AF Mode (10)

    GPIOA_PUPDR &= ~(3UL << (2 * 0));
    GPIOA_PUPDR |=  (1UL << (2 * 0)); // Internal Pull-Up

    // Set AF2 / AF1 depending on target family (AF2 = 0x2)
    GPIOA_AFRL  &= ~(0xFUL << (4 * 0));
    GPIOA_AFRL  |=  (0x2UL << (4 * 0));

    // 3. PA5 as Output (LED)
    GPIOA_MODER &= ~(3UL << (2 * 5));
    GPIOA_MODER |=  (1UL << (2 * 5));
    GPIOA_ODR   &= ~(1 << 5);         // Initially OFF

    // 4. TIM2 Prescaler (1 MHz tick = 1us resolution at 8MHz sysclk)
    TIM2->PSC = 8 - 1;
    TIM2->ARR = 0xFFFFFFFF;

    // 5. Start Timer
    TIM2->CR1 |= (1 << 0);

    uint32_t t_press = 0;
    uint32_t t_release = 0;
    uint8_t button_was_pressed = 0;

    while (1)
    {
        uint8_t current_state = !(GPIOA_IDR & (1 << 0)); // 1 if pressed, 0 if released

        if (current_state && !button_was_pressed)
        {
            // Button transition: JUST PRESSED
            t_press = TIM2->CNT;    // Capture timer value
            GPIOA_ODR |= (1 << 5);  // Turn LED ON
            button_was_pressed = 1;
        }
        else if (!current_state && button_was_pressed)
        {
            // Button transition: JUST RELEASED
            t_release = TIM2->CNT;  // Capture timer value
            GPIOA_ODR &= ~(1 << 5); // Turn LED OFF

            // Calculate duration in microseconds
            if (t_release >= t_press) {
                press_duration_us = t_release - t_press;
            } else {
                press_duration_us = (0xFFFFFFFF - t_press) + t_release + 1;
            }

            button_was_pressed = 0;
        }
    }
}