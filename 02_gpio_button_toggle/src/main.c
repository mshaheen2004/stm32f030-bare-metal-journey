/**
 * @file main.c
 * @author Mohammed Shaheen K S
 * @brief Project 02: GPIO Push-Button Toggle with Software Debounce for STM32F030C8T6
 * @details Reads a push-button input on PA0 (with internal pull-up enabled) and toggles 
 *          an LED on PA9 upon press detection, incorporating full software debounce 
 *          and release-waiting logic without HAL or CMSIS dependencies.
 */

#include <stdint.h>

// Hardware Register Addresses for STM32F030 (Port A)
#define RCC_AHBENR   (*(volatile uint32_t *)0x40021014)

#define GPIOA_MODER  (*(volatile uint32_t *)0x48000000)
#define GPIOA_PUPDR  (*(volatile uint32_t *)0x4800000C)
#define GPIOA_IDR    (*(volatile uint32_t *)0x48000010)
#define GPIOA_BSRR   (*(volatile uint32_t *)0x48000018)

void delay(volatile uint32_t count) {
    while (count--) {
        __asm("nop");
    }
}

int main(void)
{
    uint8_t led_state = 0; // 0 = OFF, 1 = ON

    // 1. Enable GPIOA Clock (Bit 17)
    RCC_AHBENR |= (1 << 17);
    for (volatile int i = 0; i < 1000; i++); // Settling delay

    // 2. Configure PA9 as Output Mode (01) - For LED
    GPIOA_MODER &= ~(3UL << (2 * 9));
    GPIOA_MODER |=  (1UL << (2 * 9));

    // 3. Configure PA0 as Input Mode (00) - For Button
    GPIOA_MODER &= ~(3UL << (2 * 0));

    // 4. Enable Internal Pull-Up Resistor on PA0 (01)
    GPIOA_PUPDR &= ~(3UL << (2 * 0));
    GPIOA_PUPDR |=  (1UL << (2 * 0));

    // Initial State: LED OFF (PA9 HIGH)
    GPIOA_BSRR = (1 << 9);

    while (1)
    {
        // Check if button is PRESSED (PA0 connected to GND -> Reads LOW / 0)
        if ((GPIOA_IDR & (1 << 0)) == 0)
        {
            // Software Debounce: wait to confirm it's a real press
            delay(100000);

            // Double-check button is STILL pressed after delay
            if ((GPIOA_IDR & (1 << 0)) == 0)
            {
                // Toggle state
                led_state = !led_state;

                // Update LED
                if (led_state) {
                    GPIOA_BSRR = (1 << (9 + 16)); // LED ON
                } else {
                    GPIOA_BSRR = (1 << 9);        // LED OFF
                }

                // WAIT HERE until button is RELEASED so it doesn't blink while holding!
                while ((GPIOA_IDR & (1 << 0)) == 0) {
                    __asm("nop");
                }

                // Debounce release
                delay(100000);
            }
        }
    }
}