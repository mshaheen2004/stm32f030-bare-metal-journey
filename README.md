# STM32F030C8T6 Bare-Metal Embedded Journey 🚀

Welcome to my personal collection of bare-metal embedded systems projects built around the **STM32F030C8T6** (ARM Cortex-M0) microcontroller. 

This repository documents my hands-on learning path in register-level embedded programming, moving from basic peripheral control to advanced hardware features using **zero heavy abstraction layers (no HAL/CMSIS dependencies beyond standard memory maps and `<stdint.h>`)**.

---

## 📂 Repository Structure

Each project in this repository is organized into its own self-contained folder containing its respective source code and documentation. 

```text
stm32f030-bare-metal-journey/
│
├── README.md              # Main landing page (this file)
├── LICENSE                # MIT Open-Source License
│
├── project_one_folder/    # Individual project workspace
│   ├── README.md          # Project-specific details & wiring
│   └── src/
│       └── main.c
│
└── project_two_folder/    # Future project workspace
    ├── README.md
    └── src/
        └── main.c