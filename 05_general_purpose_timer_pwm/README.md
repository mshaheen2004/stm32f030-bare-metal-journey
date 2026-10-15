# Project 05: General-Purpose Timer (TIM3) Hardware PWM & Breathing LED (STM32F030)

A register-level bare-metal program written for the **STM32F030C8T6** microcontroller utilizing the **TIM3 general-purpose timer** peripheral to generate a hardware-driven Pulse Width Modulation (PWM) signal on pin **PA6**, producing a smooth fading (breathing) LED effect.

---

## 🚀 Project Overview

Transitioning from core system ticks and software loops, this project explores advanced hardware peripherals by programming a 16-bit general-purpose timer for automated waveform generation. It demonstrates:
1. **General-Purpose Timer Configuration (`TIM3`):** Enabling APB1 bus clocks and structuring register memory maps for advanced peripheral blocks.
2. **Alternate Function Muxing (`AFRL`):** Routing internal timer output channels (`TIM3_CH1`) to physical MCU pins (`PA6`) using alternate function mappings (`AF1`).
3. **PWM Timebase Calculation (`PSC` & `ARR`):** Computing prescaler and auto-reload values based on the $8\text{ MHz}$ internal clock to achieve a stable $1\text{ kHz}$ PWM frequency.
4. **PWM Mode 1 Setup (`CCMR1`):** Setting output compare behavior and enabling preload registers to prevent timing glitches during updates.
5. **Dynamic Duty Cycle Modulation:** Continuously shifting the compare register (`CCR1`) inside the main loop to alter pulse widths, creating a gradual visual fading effect.

---

## 📦 Hardware Components & Bill of Materials (BOM)

| Item | Component Description | Quantity | Purpose |
| :--- | :--- | :--- | :--- |
| **1** | STM32F030C8T6 Development Board | 1 | Microcontroller core |
| **2** | ST-LINK V2 Debugger / Programmer | 1 | For flashing and debugging via SWD |
| **3** | Standard LED (Any color) | 1 | Visual output status indicator |
| **4** | $220\,\Omega \text{ to } 1\text{ k}\Omega$ Resistor | 1 | Current limiter for the LED |
| **5** | Breadboard & Jumper Wires | As needed | Circuit prototyping connections |

---

## 🔌 Hardware Circuit Connections

| Component | MCU Pin | Pin Name | External Circuit Connection | Behavior |
| :--- | :--- | :--- | :--- | :--- |
| **LED** | Pin 22 / PA6 | **PA6** | Anode (+) $\rightarrow$ PA6 <br> Cathode (-) $\rightarrow$ Resistor $\rightarrow$ **GND** | Automatically pulses brightness smoothly (breathing effect) via general-purpose timer hardware PWM |
| **Common Ground** | — | **GND** | Board Ground $\rightarrow$ Power Supply Ground | Common reference |

---

## 🧠 How the Code Works Under the Hood

### 1. Timer Frequency Math (`PSC` & `ARR`)
* **Prescaler (`PSC = 8 - 1`):** Scales the $8\text{ MHz}$ HSI clock down to $1\text{ MHz}$, resulting in a precise counter resolution of $1\,\mu\text{s}$ per tick.
* **Auto-Reload (`ARR = 1000 - 1`):** Sets the counter upper limit to $999$, establishing a full period length of $1000\,\mu\text{s}$ ($1\text{ ms}$), which equals a $1\text{ kHz}$ output frequency.

### 2. Output Compare Control (`CCMR1`)
* Setting `OC1M = 110` selects **PWM Mode 1**, holding the output pin HIGH while the counter value `CNT < CCR1`, and dropping it LOW when `CNT >= CCR1`.
* Setting `OC1PE = 1` activates the preload register so that modifications to `CCR1` apply cleanly on the next timer update event.

### 3. Dynamic Sweeping Loop
* The main loop increments and decrements the `CCR1` variable between $0$ and $1000$ with a brief delay, smoothly adjusting the average power delivered to the LED.

---

## 📂 File Structure

```text
05_hardware_pwm_breathing_led/
├── README.md      # Documentation, BOM, and wiring guide (this file)
└── src/
    └── main.c     # Bare-metal C source code with author header