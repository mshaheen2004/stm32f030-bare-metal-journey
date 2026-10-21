/**
 * @file main.c
 * @author Mohammed Shaheen K S
 * @brief Project 09: Bare-Metal ADC Potentiometer Controlled LED PWM Dimming for STM32F030C8T6
 * @details Samples a 10k potentiometer on PA1 via ADC1, mapping the 12-bit raw value (0-4095)
 *          to TIM1 Channel 2 PWM duty cycle on PA9 to control LED brightness dynamically.
 */

#include <stdint.h>

// ============================================================================
// REGISTER DEFINITIONS
// ============================================================================
#define RCC_AHBENR       (*(volatile uint32_t *)0x40021014)
#define RCC_APB2ENR      (*(volatile uint32_t *)0x40021018)

#define GPIOA_MODER      (*(volatile uint32_t *)0x48000000)
#define GPIOA_AFRH       (*(volatile uint32_t *)0x48000024)

// ADC1 Registers (Base: 0x40012400)
typedef struct {
    volatile uint32_t ISR;
    volatile uint32_t IER;
    volatile uint32_t CR;
    volatile uint32_t CFGR1;
    volatile uint32_t CFGR2;
    volatile uint32_t SMPR;
    volatile uint32_t RESERVED[2];
    volatile uint32_t WTR;
    volatile uint32_t TR;
    volatile uint32_t CHSELR;
    volatile uint32_t RESERVED2[5];
    volatile uint32_t DR;
} ADC_TypeDef;

#define ADC1 ((ADC_TypeDef *) 0x40012400)

// TIM1 Registers (Base: 0x40012C00)
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
    volatile uint32_t CCR3;
    volatile uint32_t CCR4;
    volatile uint32_t BDTR;
} TIM_TypeDef;

#define TIM1 ((TIM_TypeDef *) 0x40012C00)

// Global volatile variables for Live Expressions debugging
volatile uint32_t adc_raw = 0;
volatile uint32_t voltage_mv = 0;
volatile uint32_t pwm_duty = 0;

void TIM1_PWM_Init(void) {
    // Step 1: Enable TIM1 and GPIOA clocks
    RCC_APB2ENR |= (1 << 11); // Enable TIM1 clock
    RCC_AHBENR  |= (1 << 17); // Enable GPIOA clock

    // Step 2: Configure PA9 as Alternate Function (TIM1_CH2 = AF2)
    GPIOA_MODER &= ~(3UL << (2 * 9));
    GPIOA_MODER |=  (2UL << (2 * 9));   // Alternate Function mode

    // PA9 uses AFRH (Pins 8 to 15). Pin 9 is bits [7:4], AF2 = 0x2
    GPIOA_AFRH  &= ~(0xFUL << (4 * (9 - 8)));
    GPIOA_AFRH  |=  (0x2UL << (4 * (9 - 8)));

    // Step 3: Configure Timer Base for PWM Frequency (~1 kHz at 8 MHz HSI)
    // Timer Clock = 8 MHz. Prescaler = 8 - 1 = 7 (Timer clock = 1 MHz)
    // Auto-Reload Register (ARR) = 1000 - 1 = 999 (PWM Frequency = 1 MHz / 1000 = 1 kHz)
    TIM1->PSC = 8 - 1;
    TIM1->ARR = 1000 - 1;

    // Step 4: Configure TIM1 Channel 2 as PWM Mode 1 (OCM = 110)
    // CCMR1 Channel 2 bits [14:12] = 110 (PWM Mode 1), and enable preload (bit 11)
    TIM1->CCMR1 |= (6UL << 12) | (1UL << 3);

    // Step 5: Enable Output for Channel 2 (CC2E = 1) and set active high polarity
    TIM1->CCER |= (1 << 4);

    // Step 6: Enable Advanced-Timer Main Output (MOE = 1)
    TIM1->BDTR |= (1 << 15);

    // Step 7: Enable TIM1 Counter (CEN = 1)
    TIM1->CR1 |= (1 << 0);
}

int main(void)
{
    // Step 1: Enable Clocks for GPIOA and ADC1
    RCC_AHBENR  |= (1 << 17);
    RCC_APB2ENR |= (1 << 9);  // Enable ADC clock

    // Step 2: Configure PA1 as Analog Mode (11) for ADC Input (ADC_IN1)
    GPIOA_MODER |= (3UL << (2 * 1));

    // Step 3: Initialize TIM1 PWM Output on PA9
    TIM1_PWM_Init();

    // Step 4: ADC Calibration & Startup
    if (ADC1->CR & (1 << 31)) {
        ADC1->CR |= (1 << 30); // Disable ADC if enabled
        while (ADC1->CR & (1 << 31));
    }

    ADC1->CR |= (1 << 31);     // Start Calibration
    while (ADC1->CR & (1 << 31)); // Wait for calibration to complete

    ADC1->CR |= (1 << 0);      // Enable ADC (ADEN = 1)
    while (!(ADC1->ISR & (1 << 0))); // Wait for ADRDY flag

    // Select ADC Channel 1 (PA1)
    ADC1->CHSELR = (1 << 1);

    while (1)
    {
        // Start ADC Conversion
        ADC1->CR |= (1 << 2); // ADSTART = 1

        // Wait for End of Conversion (EOC)
        while (!(ADC1->ISR & (1 << 2)));

        // Read 12-bit ADC Raw Value (0 to 4095)
        adc_raw = ADC1->DR;

        // Convert raw value to millivolts: (adc_raw * 3300) / 4095
        voltage_mv = (adc_raw * 3300) / 4095;

        // Map 12-bit ADC range (0-4095) to Timer PWM duty cycle range (0-999)
        pwm_duty = (adc_raw * 999) / 4095;

        // Update PWM pulse width register
        TIM1->CCR2 = pwm_duty;
    }
}