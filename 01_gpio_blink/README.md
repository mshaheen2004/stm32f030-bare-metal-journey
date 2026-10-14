# Project 01: Bare-Metal GPIO LED Blink (STM32F030)

A zero-dependency, register-level bare-metal program written for the **STM32F030C8T6** microcontroller to blink an LED connected to **Pin PA9** using direct memory mapping and bit-band operations.

---

## 🚀 Project Overview

This project serves as the foundation for bare-metal embedded programming, bypassing all heavy Hardware Abstraction Layers (HAL/CMSIS) to interact directly with internal microcontroller registers. It demonstrates:
1. **Clock Enable Control:** Powering on GPIO Port A via the Reset and Clock Control (`RCC`) register.
2. **Pin Mode Configuration:** Configuring a GPIO pin as a general-purpose digital output using the Mode Register (`MODER`).
3. **Atomic Pin Manipulation:** Toggling pins efficiently using the Bit Set/Reset Register (`BSRR`) instead of rewriting the entire Output Data Register (`ODR`).
4. **Software Delays:** Implementing a basic loop delay with inline assembly (`nop`).

---

## 📦 Hardware Components & Bill of Materials (BOM)

| Item | Component Description | Quantity | Purpose |
| :--- | :--- | :--- | :--- |
| **1** | STM32F030C8T6 Development Board | 1 | Microcontroller core |
| **2** | ST-LINK V2 Debugger / Programmer | 1 | For flashing and debugging code via SWD |
| **3** | Standard LED (Any color) | 1 | Visual output indicator |
| **4** | $220\,\Omega \text{ to } 1\text{ k}\Omega$ Resistor | 1 | Current limiter to protect the GPIO pin |
| **5** | Breadboard & Jumper Wires | As needed | Circuit prototyping connections |

---

## 🔌 Hardware Circuit Connections

| Component | MCU Pin | Pin Name | External Circuit Connection | Behavior |
| :--- | :--- | :--- | :--- | :--- |
| **LED** | Pin 17 | **PA9** | Anode (+) $\rightarrow$ PA9 <br> Cathode (-) $\rightarrow$ Resistor $\rightarrow$ **GND** | Blinks continuously (ON for ~300ms, OFF for ~300ms) |
| **Common Ground** | — | **GND** | Board Ground $\rightarrow$ Power Supply Ground | Common reference |

---

## 🧠 How the Code Works Under the Hood

### 1. Clock Enable (`RCC_AHBENR`)
By default, internal clocks to all GPIO ports are disabled to save power. 
- Bit 17 of `RCC_AHBENR` controls the clock for **GPIO Port A**. Setting `RCC_AHBENR |= (1 << 17)` turns on the hardware block so its registers can accept read/write instructions.

### 2. Pin Mode Configuration (`GPIOA_MODER`)
Each GPIO pin has 2 bits in the `MODER` register controlling its behavior (00 = Input, 01 = Output, 10 = Alternate Function, 11 = Analog).
- For **PA9**, bits 18 and 19 (`2 * 9`) are cleared and then set to `01` to configure it as a digital output.

### 3. Efficient Pin Toggling (`GPIOA_BSRR`)
Instead of reading, modifying, and writing back to the Output Data Register (`ODR`)—which can cause race conditions—the **Bit Set/Reset Register (`BSRR`)** allows single-cycle atomic writes:
- **Writing to the upper 16 bits (Bits 16–31):** Resets the corresponding pin to **LOW**.
- **Writing to the lower 16 bits (Bits 0–15):** Sets the corresponding pin to **HIGH**.
- In the code, `PA9 LOW` is triggered by writing to bit index `9 + 16` (25), and `PA9 HIGH` is triggered by writing directly to bit `9`.

---

## 📂 File Structure

```text
01_gpio_blink/
├── README.md      # Documentation, BOM, and wiring guide (this file)
└── src/
    └── main.c     # Bare-metal C source code