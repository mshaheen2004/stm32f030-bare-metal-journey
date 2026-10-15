# Project 06: Advanced-Control Timer (TIM1) Complementary PWM & Dead-Time Insertion (STM32F030)

A register-level bare-metal program written for the **STM32F030C8T6** microcontroller utilizing the **TIM1 advanced-control timer** peripheral to generate complementary PWM waveforms with hardware dead-time control on pins **PA8** and **PA7**.

---

## 🚀 Project Overview

Stepping up from general-purpose timers, this project leverages STM32's **Advanced-Control Timer (`TIM1`)**, which includes specialized power-electronics features like complementary output pairs and dead-time generation. It demonstrates:
1. **Advanced-Control Timer Architecture (`TIM1`):** Interacting with high-speed APB2 bus timer registers designed for motor control and power conversion.
2. **Complementary Output Pin Muxing:** Mapping primary channel outputs (`CH1` on `PA8`) and inverse/complementary outputs (`CH1N` on `PA7`) using Alternate Function 2 (`AF2`).
3. **Dead-Time Insertion (`BDTR`):** Programming the Break and Dead-Time Register to inject a safe non-overlap delay ($\Delta t = 50\,\mu\text{s}$) to prevent shoot-through currents in bridge topologies.
4. **Main Output Enable (`MOE`):** Setting the mandatory MOE bit in the `BDTR` register to unlock driver outputs on advanced-control timers.
5. **Dynamic Dual-Channel Fading:** Sweeping the capture/compare register (`CCR1`) to drive reciprocal brightness transitions across complementary outputs.

---

## 📦 Hardware Components & Bill of Materials (BOM)

| Item | Component Description | Quantity | Purpose |
| :--- | :--- | :--- | :--- |
| **1** | STM32F030C8T6 Development Board | 1 | Microcontroller core |
| **2** | ST-LINK V2 Debugger / Programmer | 1 | For flashing and debugging via SWD |
| **3** | Standard LEDs (Any color) | 2 | Visual output indicators for primary and complementary signals |
| **4** | $220\,\Omega \text{ to } 1\text{ k}\Omega$ Resistors | 2 | Current limiters for the LEDs |
| **5** | Breadboard & Jumper Wires | As needed | Circuit prototyping connections |

---

## 🔌 Hardware Circuit Connections

| Component | MCU Pin | Pin Name | External Circuit Connection | Behavior |
| :--- | :--- | :--- | :--- | :--- |
| **LED 1 (Primary)** | Pin 39 / PA8 | **PA8 (TIM1_CH1)** | Anode (+) $\rightarrow$ PA8 <br> Cathode (-) $\rightarrow$ Resistor $\rightarrow$ **GND** | Pulso-fades dynamically based on the active duty cycle |
| **LED 2 (Complementary)** | Pin 25 / PA7 | **PA7 (TIM1_CH1N)** | Anode (+) $\rightarrow$ PA7 <br> Cathode (-) $\rightarrow$ Resistor $\rightarrow$ **GND** | Fades inversely relative to LED 1, protected by hardware dead-time |
| **Common Ground** | — | **GND** | Board Ground $\rightarrow$ Power Supply Ground | Common reference |

---

## 🧠 How the Code Works Under the Hood

### 1. Complementary Pin Mapping (`AFRH` & `AFRL`)
* Unlike standard timers, `TIM1` features dedicated complementary outputs (`CH1N`). Pin `PA8` is mapped to `AF2` via `AFRH`, while `PA7` is mapped to `AF2` via `AFRL` to supply the inverted drive signal.

### 2. Dead-Time & Main Output Enable (`BDTR`)
* **Dead-Time Generator (`DTG`):** The lower bits of the Break and Dead-Time Register (`BDTR`) are set to `50`, injecting a safety delay between high-side and low-side switching transitions.
* **Main Output Enable (`MOE`):** Bit 15 of `BDTR` acts as a master switch. Unlike general-purpose timers, advanced timers require `MOE = 1` before any PWM signal reaches physical pins.

---

## 📂 File Structure

```text
06_advanced_timer_complementary_pwm/
├── README.md      # Documentation, BOM, and wiring guide (this file)
└── src/
    └── main.c     # Bare-metal C source code with author header