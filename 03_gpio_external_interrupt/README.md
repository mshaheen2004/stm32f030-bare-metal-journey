# Project 03: Bare-Metal External Interrupts (EXTI) & Push-Button ISR (STM32F030)

A register-level bare-metal program written for the **STM32F030C8T6** microcontroller implementing hardware-driven **External Interrupts (EXTI)** to toggle an LED (`PA11`) via a push-button (`PC14`) without CPU polling loops.

---

## 🚀 Project Overview

Moving away from CPU-heavy polling loops (`while` checks), this project introduces event-driven programming using hardware interrupts. It demonstrates:
1. **Multi-Port Clock Enable:** Activating GPIO Port A, GPIO Port C, and the System Configuration Controller (`SYSCFG`) via `RCC_AHBENR`.
2. **Input Multiplexing (`SYSCFG`):** Routing physical port pins (like `PC14`) to external interrupt lines (`EXTI14`) using the External Interrupt Configuration Register (`EXTI_CR`).
3. **Edge Trigger Selection:** Configuring the Falling Trigger Selection Register (`FTSR`) to react instantly when a button press pulls the input line from HIGH to LOW.
4. **Interrupt Masking & NVIC Routing:** Unmasking interrupt lines (`EXTI_IMR`) and enabling the appropriate vector position in the Nested Vectored Interrupt Controller (`NVIC_ISER`).
5. **Interrupt Service Routines (ISRs):** Handling events asynchronously, clearing pending flags (`EXTI_PR`), and keeping the `while(1)` main loop completely free.

---

## 📦 Hardware Components & Bill of Materials (BOM)

| Item | Component Description | Quantity | Purpose |
| :--- | :--- | :--- | :--- |
| **1** | STM32F030C8T6 Development Board | 1 | Microcontroller core |
| **2** | ST-LINK V2 Debugger / Programmer | 1 | For flashing and debugging via SWD |
| **3** | Standard LED (Any color) | 1 | Visual output status indicator |
| **4** | Tactile Push-Button Switch | 1 | External trigger input source |
| **5** | $220\,\Omega \text{ to } 1\text{ k}\Omega$ Resistor | 1 | Current limiter for the LED |
| **6** | Breadboard & Jumper Wires | As needed | Circuit prototyping connections |

---

## 🔌 Hardware Circuit Connections

| Component | MCU Pin | Pin Name | External Circuit Connection | Behavior |
| :--- | :--- | :--- | :--- | :--- |
| **LED** | Pin 42 (approx.) / check layout | **PA11** | Anode (+) $\rightarrow$ PA11 <br> Cathode (-) $\rightarrow$ Resistor $\rightarrow$ **GND** | Toggles state (ON/OFF) asynchronously on button press |
| **Push-Button** | Pin 3 (approx.) / check layout | **PC14** | One terminal $\rightarrow$ PC14 <br> Other terminal $\rightarrow$ **GND** | Pulls pin LOW on press, triggering hardware interrupt via internal pull-up |
| **Common Ground** | — | **GND** | Board Ground $\rightarrow$ Power Supply Ground | Common reference |

---

## 🧠 How the Code Works Under the Hood

### 1. Multiplexing EXTI Lines (`SYSCFG_EXTICR4`)
* By default, EXTI line 14 can listen to PA14, PB14, or PC14. The code writes `0x2` to `SYSCFG_EXTICR4` to bind **PC14** explicitly to **EXTI14**.

### 2. Edge Trigger & Unmasking (`EXTI_FTSR` & `EXTI_IMR`)
* `EXTI_FTSR` is configured for falling-edge detection (detecting the transition when the button pulls the pin from $3.3\text{V}$ to $0\text{V}$).
* `EXTI_IMR` unmasks the line so the signal is allowed to propagate to the core processor interrupt controller.

### 3. NVIC Integration (`NVIC_ISER`)
* On STM32F030 devices, external lines 4 through 15 share a single combined interrupt vector (`EXTI4_15_IRQHandler`). Setting bit 7 of `NVIC_ISER` enables this vector group in the ARM Cortex-M0 processor.

### 4. Clearing Pending Flags (`EXTI_PR`)
* Inside the ISR, once the event is handled, **writing a `1` to the pending register flag (`EXTI_PR`)** clears the interrupt request. If omitted, the MCU will get trapped in an infinite interrupt loop.

---

## 📂 File Structure

```text
03_gpio_external_interrupt/
├── README.md      # Documentation, BOM, and wiring guide (this file)
└── src/
    └── main.c     # Bare-metal C source code with author header