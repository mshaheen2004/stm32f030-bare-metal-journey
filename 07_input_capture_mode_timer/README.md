# Project 07: General-Purpose Timer Input Capture Mode & Pulse-Width Measurement (STM32F030)

A register-level bare-metal program written for the **STM32F030C8T6** microcontroller utilizing a general-purpose timer (**TIM2**) to measure external pulse lengths and button press durations with microsecond precision.

---

## 🚀 Project Overview

Moving from waveform generation to time measurement, this project implements input capture techniques using a 32-bit general-purpose timer (`TIM2`). It demonstrates:
1. **General-Purpose Timer Configuration (`TIM2`):** Activating APB1 bus clocks and scaling the timer timebase to achieve precise microsecond counts.
2. **Prescaler & Auto-Reload Setup:** Configuring a $1\,\mu\text{s}$ tick resolution ($8\text{ MHz} / 8$) with a maximum 32-bit range (`0xFFFFFFFF`) to prevent counter overflow during long intervals.
3. **Pin Edge State Sampling:** Monitoring physical pin transitions (`PA0`) with internal pull-up resistors enabled.
4. **Timestamp Capture Logic:** Recording timer counter values (`TIM2->CNT`) on press and release events to calculate exact hold durations.

---

## 📦 Hardware Components & Bill of Materials (BOM)

| Item | Component Description | Quantity | Purpose |
| :--- | :--- | :--- | :--- |
| **1** | STM32F030C8T6 Development Board | 1 | Microcontroller core |
| **2** | ST-LINK V2 Debugger / Programmer | 1 | For flashing and debugging via SWD |
| **3** | Tactile Push-Button Switch | 1 | Input trigger source for duration measurement |
| **4** | Standard LED (Any color) | 1 | Visual activity indicator on PA5 |
| **5** | $220\,\Omega \text{ to } 1\text{ k}\Omega$ Resistor | 1 | Current limiter for the LED |
| **6** | Breadboard & Jumper Wires | As needed | Circuit prototyping connections |

---

## 🔌 Hardware Circuit Connections

| Component | MCU Pin | Pin Name | External Circuit Connection | Behavior |
| :--- | :--- | :--- | :--- | :--- |
| **Push-Button** | Pin 6 / PA0 | **PA0 (TIM2_CH1)** | One terminal $\rightarrow$ PA0 <br> Other terminal $\rightarrow$ **GND** | Pulls pin LOW on press, triggering pulse-width capture |
| **Indicator LED** | Pin 11 / PA5 | **PA5** | Anode (+) $\rightarrow$ PA5 <br> Cathode (-) $\rightarrow$ Resistor $\rightarrow$ **GND** | Illuminates while the push-button is actively held down |
| **Common Ground** | — | **GND** | Board Ground $\rightarrow$ Power Supply Ground | Common reference |

---

## 🧠 How the Code Works Under the Hood

### 1. Timer Resolution (`PSC` & `ARR`)
* **Prescaler (`PSC = 8 - 1`):** Divides the $8\text{ MHz}$ system clock down to $1\text{ MHz}$, providing exactly $1\,\mu\text{s}$ per timer tick.
* **Auto-Reload (`ARR = 0xFFFFFFFF`):** Maximizes the counter ceiling to support long measurement windows without rollover errors.

### 2. Duration Computation
* Upon a press transition, the current counter value (`TIM2->CNT`) is saved in `t_press`. Upon release, `t_release` is recorded, and the difference is calculated, accounting for any 32-bit counter wrap-around.

---

## 📂 File Structure

```text
07_input_capture_mode_timer/
├── README.md      # Documentation, BOM, and wiring guide (this file)
└── src/
    └── main.c     # Bare-metal C source code with author header