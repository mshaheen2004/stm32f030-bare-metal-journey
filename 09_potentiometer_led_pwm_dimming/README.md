# Project 09: Potentiometer-Controlled LED PWM Dimming (STM32F030)

A register-level bare-metal program written for the **STM32F030C8T6** microcontroller that dynamically controls LED brightness by linking an analog potentiometer input (ADC1) to an advanced timer PWM output (TIM1 Channel 2) entirely in hardware.

---

## 🚀 Project Overview

Transitioning from basic analog sampling to dynamic hardware actuation, this project demonstrates real-time feedback control without CPU intervention. It demonstrates:
1. **Analog-to-Digital Conversion (`ADC1`):** Calibrating and sampling a 10k potentiometer on **PA1** (`ADC_IN1`), yielding a 12-bit digital value (`0`–`4095`).
2. **Advanced Timer PWM Generation (`TIM1`):** Configuring Timer 1 to produce a stable **1 kHz PWM frequency** on pin **PA9** (`TIM1_CH2`) with 1,000 steps of duty cycle resolution (`0`–`999`).
3. **Real-Time Mapping:** Linearly scaling the 12-bit ADC raw range into the timer's compare register (`TIM1->CCR2`) inside a high-speed execution loop.
4. **Live Expressions Debugging:** Tracking `adc_raw`, `voltage_mv`, and `pwm_duty` in real-time through the debugger interface without requiring an external UART serial bridge.

---

## 📦 Hardware Components & Bill of Materials (BOM)

| Item | Component Description | Quantity | Purpose |
| :--- | :--- | :--- | :--- |
| **1** | STM32F030C8T6 Development Board | 1 | Microcontroller core |
| **2** | ST-LINK V2 Debugger / Programmer | 1 | For flashing and debugging via SWD |
| **3** | 10k $\Omega$ Rotary Potentiometer | 1 | Analog voltage divider input device |
| **4** | Standard LED (Any color) | 1 | Visual brightness indicator output |
| **5** | $220\,\Omega \text{ to } 1\text{ k}\Omega$ Resistor | 1 | Current limiter for the LED |
| **6** | Breadboard & Jumper Wires | As needed | Circuit prototyping connections |

---

## 🔌 Hardware Circuit Connections

| Component | MCU Pin | Pin Name | External Circuit Connection | Behavior |
| :--- | :--- | :--- | :--- | :--- |
| **Potentiometer VCC** | Pin 16 | **3.3V** | Potentiometer Outer Pin 1 $\rightarrow$ **3.3V** | Power rail supply |
| **Potentiometer Wiper** | Pin 11 | **PA1 (`ADC_IN1`)** | Potentiometer Middle Wiper $\rightarrow$ **PA1** | Provides variable analog voltage (0V to 3.3V) |
| **Potentiometer GND** | Pin 8 / GND | **GND** | Potentiometer Outer Pin 3 $\rightarrow$ **GND** | Ground reference |
| **LED Driver Output** | Pin 30 / PA9 | **PA9 (`TIM1_CH2`)** | Anode (+) $\rightarrow$ PA9 <br> Cathode (-) $\rightarrow$ Resistor $\rightarrow$ **GND** | Dims proportionally as the potentiometer knob is rotated |
| **Common Ground** | — | **GND** | Board Ground $\rightarrow$ Power Supply Ground | Common reference |

---

## 🧠 How the Code Works Under the Hood

### 1. ADC Calibration & Startup (`ADC1`)
- Before sampling, the code checks if the ADC is enabled (`ADEN`), shuts it down via `ADDIS` if needed, and triggers hardware offset calibration (`ADCAL`) to ensure precise readings.
- Channel selection is locked to `ADC_IN1` (`CHSELR = (1 << 1)`).

### 2. Timer PWM Configuration (`TIM1`)
- The timer prescaler (`PSC = 8 - 1`) divides the 8 MHz HSI clock down to a 1 MHz counter clock ($1\ \mu\text{s}$ per tick).
- The auto-reload register (`ARR = 1000 - 1`) establishes a 1,000-tick period, yielding a steady **1 kHz PWM frequency**.
- Main output configuration enables PWM Mode 1, channel output drivers (`CC2E`), and the advanced timer's Main Output Enable (`MOE` via `BDTR`).

### 3. Real-Time Conversion Loop (`while(1)`)
- Manually triggers an ADC conversion (`ADSTART`), polls the `EOC` flag, and fetches the 12-bit result from `ADC1->DR`.
- Scales the raw value into millivolts for debugging and maps it proportionally into `TIM1->CCR2` to instantly adjust the LED pulse-width.

---

## 📂 File Structure

```text
09_potentiometer_led_pwm_dimming/
├── README.md      # Documentation, BOM, and wiring guide (this file)
└── src/
    └── main.c     # Bare-metal C source code with author header