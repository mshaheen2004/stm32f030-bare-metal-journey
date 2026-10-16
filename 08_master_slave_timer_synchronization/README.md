# Project 08: Master-Slave Timer Hardware Synchronization (TIM1 & TIM3) (STM32F030)

A register-level bare-metal program written for the **STM32F030C8T6** microcontroller synchronizing two hardware timers (**TIM1** as Master and **TIM3** as Slave) entirely in hardware without CPU intervention.

---

## 🚀 Project Overview

Moving into advanced multi-timer configurations, this project demonstrates inter-timer triggering where a master timer controls the exact reset timing of a slave timer via internal trigger channels (`ITR0`). It demonstrates:
1. **Master Timer Configuration (`TIM1`):** Setting up an advanced-control timer to generate trigger pulses (`TRGO`) on update events via Master Mode Selection (`MMS`).
2. **Slave Timer Configuration (`TIM3`):** Configuring a general-purpose timer in **Reset Mode** (`SMS = 100`) to automatically clear its counter to zero whenever a master trigger pulse is received.
3. **Internal Trigger Routing (`ITR0`):** Mapping slave trigger source selections (`TS = 000`) to link TIM1's output directly into TIM3 without external wiring.
4. **Hardware Cascading:** Achieving synchronous multi-channel PWM outputs on pins **PA8** and **PA6** with zero software jitter.

---

## 📦 Hardware Components & Bill of Materials (BOM)

| Item | Component Description | Quantity | Purpose |
| :--- | :--- | :--- | :--- |
| **1** | STM32F030C8T6 Development Board | 1 | Microcontroller core |
| **2** | ST-LINK V2 Debugger / Programmer | 1 | For flashing and debugging via SWD |
| **3** | Standard LEDs (Any color) | 2 | Visual output indicators for master and slave signals |
| **4** | $220\,\Omega \text{ to } 1\text{ k}\Omega$ Resistors | 2 | Current limiters for the LEDs |
| **5** | Breadboard & Jumper Wires | As needed | Circuit prototyping connections |

---

## 🔌 Hardware Circuit Connections

| Component | MCU Pin | Pin Name | External Circuit Connection | Behavior |
| :--- | :--- | :--- | :--- | :--- |
| **Master LED** | Pin 39 / PA8 | **PA8 (TIM1_CH1)** | Anode (+) $\rightarrow$ PA8 <br> Cathode (-) $\rightarrow$ Resistor $\rightarrow$ **GND** | Blinks at 1 Hz with a 50% duty cycle (500ms ON / 500ms OFF) |
| **Slave LED** | Pin 22 / PA6 | **PA6 (TIM3_CH1)** | Anode (+) $\rightarrow$ PA6 <br> Cathode (-) $\rightarrow$ Resistor $\rightarrow$ **GND** | Resets and flashes synchronously with the master timer every second |
| **Common Ground** | — | **GND** | Board Ground $\rightarrow$ Power Supply Ground | Common reference |

---

## 🧠 How the Code Works Under the Hood

### 1. Master Trigger Output (`TIM1_CR2`)
* Setting `MMS = 010` instructs `TIM1` to emit a trigger output pulse (`TRGO`) on every update event (when the counter hits its auto-reload threshold).

### 2. Slave Reset Mode (`TIM3_SMCR`)
* Setting `TS = 000` connects `TIM3` to `ITR0` ( TIM1's trigger output). 
* Setting `SMS = 100` puts `TIM3` into **Reset Mode**, ensuring that every incoming trigger edge from TIM1 instantly resets `TIM3->CNT` back to `0`.

---

## 📂 File Structure

```text
08_master_slave_timer_synchronization/
├── README.md      # Documentation, BOM, and wiring guide (this file)
└── src/
    └── main.c     # Bare-metal C source code with author header