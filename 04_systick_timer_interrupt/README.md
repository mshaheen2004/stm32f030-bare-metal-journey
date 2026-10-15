# Project 04: Bare-Metal Cortex-M0 SysTick Timer & Periodic Interrupts (STM32F030)

A register-level bare-metal program written for the **STM32F030C8T6** microcontroller utilizing the ARM Cortex-M0 internal **SysTick Timer** to generate accurate 500ms periodic interrupts for multitasking and LED blinking.

---

## 🚀 Project Overview

Moving beyond peripheral timers, this project leverages the core system timer built directly into the ARM Cortex-M0 processor architecture (`SysTick`). It demonstrates:
1. **Core Architecture Peripherals:** Interacting directly with Cortex-M0 core registers (`SYST_CSR`, `SYST_RVR`, `SYST_CVR`) residing outside standard peripheral memory maps.
2. **Clock Cycle Calculations:** Computing reload values based on the default $8\text{ MHz}$ internal high-speed (HSI) clock to achieve exact time-base intervals ($500\text{ ms} = 4,000,000\text{ cycles}$).
3. **Timer Control Register Setup:** Enabling the counter, triggering core interrupt requests, and configuring processor clock scaling.
4. **Global Interrupt Assembly Hooks:** Unmasking processor-level interrupts using inline assembly (`cpsie i`).
5. **Standard Core Handlers:** Implementing the pre-defined `SysTick_Handler` vector hook to manage periodic execution tasks asynchronously.

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
| **LED** | Pin 16 / PA9 | **PA9** | Anode (+) $\rightarrow$ PA9 <br> Cathode (-) $\rightarrow$ Resistor $\rightarrow$ **GND** | Blinks automatically every 500ms via core SysTick interrupts |
| **Common Ground** | — | **GND** | Board Ground $\rightarrow$ Power Supply Ground | Common reference |

---

## 🧠 How the Code Works Under the Hood

### 1. SysTick Reload Value (`SYST_RVR`)
* With the default HSI clock running at $8\text{ MHz}$ ($8,000,000\text{ cycles per second}$), a $500\text{ ms}$ ($\frac{1}{2}\text{ second}$) delay requires exactly $4,000,000$ clock ticks. Setting `SYST_RVR = 4000000 - 1` configures the down-counter threshold.

### 2. Control and Status Register (`SYST_CSR`)
* Setting Bit 0 (`ENABLE`), Bit 1 (`TICKINT` to enable core exceptions), and Bit 2 (`CLKSOURCE` for processor clock speed) initiates counting immediately.

### 3. Core Exception Handler (`SysTick_Handler`)
* Unlike peripheral interrupts, `SysTick_Handler` is a fixed exception vector built into all ARM Cortex-M microcontrollers. Whenever the reload counter hits zero, hardware vectors directly to this function to toggle the LED state cleanly.

---

## 📂 File Structure

```text
04_systick_timer_interrupt/
├── README.md      # Documentation, BOM, and wiring guide (this file)
└── src/
    └── main.c     # Bare-metal C source code with author header