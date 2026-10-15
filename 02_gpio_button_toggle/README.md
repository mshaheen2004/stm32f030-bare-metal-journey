# Project 02: Bare-Metal GPIO Push-Button Toggle with Software Debounce (STM32F030)

A register-level bare-metal program written for the **STM32F030C8T6** microcontroller to toggle an LED (`PA9`) using a tactile push-button (`PA0`), featuring internal pull-up configuration and robust software debouncing.

---

## 🚀 Project Overview

Building upon basic GPIO output control, this project introduces digital input reading, internal pull-up resistor configuration, and edge handling. It demonstrates:
1. **Input Mode Configuration:** Setting up a GPIO pin as a digital input via the Mode Register (`MODER`).
2. **Pull-Up Resistor Enable:** Utilizing internal pull-ups via the Pull-Up/Pull-Down Register (`PUPDR`) to ensure a stable logic-HIGH state when the button is unpressed.
3. **Data Input Reading:** Sampling physical pin states using the Input Data Register (`IDR`).
4. **Software Debouncing & Release Handling:** Filtering out mechanical switch bounce and waiting for button release to prevent continuous state-toggling while held down.

---

## 📦 Hardware Components & Bill of Materials (BOM)

| Item | Component Description | Quantity | Purpose |
| :--- | :--- | :--- | :--- |
| **1** | STM32F030C8T6 Development Board | 1 | Microcontroller core |
| **2** | ST-LINK V2 Debugger / Programmer | 1 | For flashing and debugging via SWD |
| **3** | Standard LED (Any color) | 1 | Visual output status indicator |
| **4** | Tactile Push-Button Switch | 1 | User input interface |
| **5** | $220\,\Omega \text{ to } 1\text{ k}\Omega$ Resistor | 1 | Current limiter for the LED |
| **6** | Breadboard & Jumper Wires | As needed | Circuit prototyping connections |

---

## 🔌 Hardware Circuit Connections

| Component | MCU Pin | Pin Name | External Circuit Connection | Behavior |
| :--- | :--- | :--- | :--- | :--- |
| **LED** | Pin 17 | **PA9** | Anode (+) $\rightarrow$ PA9 <br> Cathode (-) $\rightarrow$ Resistor $\rightarrow$ **GND** | Toggles state (ON/OFF) each time the button is pressed |
| **Push-Button** | Pin 14 | **PA0** | One terminal $\rightarrow$ PA0 <br> Other terminal $\rightarrow$ **GND** | Pulls pin LOW when pressed (internal pull-up keeps it HIGH otherwise) |
| **Common Ground** | — | **GND** | Board Ground $\rightarrow$ Power Supply Ground | Common reference |

---

## 🧠 How the Code Works Under the Hood

### 1. Input Configuration (`GPIOA_MODER`)
* For **PA0**, bits 0 and 1 are cleared (`00`), configuring the pin as a floating/digital input.

### 2. Internal Pull-Up Resistor (`GPIOA_PUPDR`)
* Because the button connects `PA0` to `GND` when pressed, an unpressed button would leave the pin floating. Setting bits 0 and 1 of `PUPDR` to `01` activates the internal **pull-up resistor**, holding `PA0` cleanly at logic-HIGH ($3.3\text{V}$) by default.

### 3. Reading Input State (`GPIOA_IDR`)
* The program continuously monitors bit 0 of `GPIOA_IDR`. When the button is pressed, it connects the pin to ground, causing the input read to drop to `0` (LOW).

### 4. Software Debouncing & State Toggle
* Mechanical buttons vibrate slightly when clicked, causing electrical noise ("bounce"). The code uses a dual-check delay loop to verify a genuine press, toggles the software `led_state` variable, updates the LED output via `BSRR`, and pauses execution until the user lets go of the button.

---

## 📂 File Structure

```text
02_gpio_button_toggle/
├── README.md      # Documentation, BOM, and wiring guide (this file)
└── src/
    └── main.c     # Bare-metal C source code with author header