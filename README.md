# Ultrasonic Radar / Sonar System (Custom PCB)

## Overview
This project is a **2D Object Localization System (Radar)** developed on a custom home-etched PCB. It utilizes an **TI MSP430G2553** microcontroller to control a servo motor for scanning and an **HC-SR04** ultrasonic sensor for distance measurement.

The firmware implements high-precision **Input Capture (Timer A)** to measure the echo pulse width from the sensor, calculating the distance to obstacles in real-time.

## Key Features
* **Custom Hardware:** Designed and fabricated a **double-layer (2-layer)** home-made PCB using the toner transfer method.
* **Sensor Fusion:** Integrates servo positioning with ultrasonic ranging to create a sector map.
* **Signal Processing:** Uses bare-metal `Timer_A` interrupts for precise pulse-width measurement (Echo signal) without blocking the CPU.
* **Data Output:** Transmits angle and distance data via UART for visualization (e.g., Radar Plotter).

## Hardware Components
| Component | Function |
| :--- | :--- |
| **MCU** | TI MSP430G2553 (DIP-20) |
| **Sensor** | HC-SR04 Ultrasonic Ranging Module |
| **Actuator** | Micro Servo (SG90 or MG90S) |
| **PCB** | **Double-Layer (2-Layer)** Custom Prototype |

## Pin Configuration
| Pin | Function | Description |
| :--- | :--- | :--- |
| **P1.1** | **Echo Input** | Connected to HC-SR04 Echo (Timer A Capture `CCI1A`) |
| **P1.2** | **UART TX** | Sends data to PC (9600 Baud) |
| **[GPIO]** | **Trig Output** | Connected to HC-SR04 Trig Pin |
| **[PWM]** | **Servo Control** | Connected to Servo Signal Pin |

## How It Works
1.  **Triggering:** The MCU generates a 10µs pulse to trigger the HC-SR04.
2.  **Echo Capture:** The sensor returns a pulse proportional to the distance. The MSP430 captures the **Rising** and **Falling** edges using hardware interrupts to calculate the "Time of Flight" precisely.
3.  **Calculation:** `Distance (cm) = (PulseTime / 2) / 29.1`
4.  **Scanning:** The servo sweeps the area (e.g., 0° to 180°), and data is sent as pairs `(Angle, Distance)` to the interface.

## Fabrication Note
This project was designed and fabricated in-house as a rapid prototyping study to demonstrate **double-layer PCB fabrication techniques** and hardware-level signal processing algorithms.
