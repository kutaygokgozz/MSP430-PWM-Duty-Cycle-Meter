# Ultrasonic Radar / Object Detector System (Custom PCB)

## Overview
This project is a **2D Object Localization & Detection System** developed on a custom home-etched PCB. It utilizes a **TI MSP430G2553** microcontroller to control a servo motor for step-by-step scanning and an **HC-SR04** ultrasonic sensor for distance measurement.

Unlike standard radars that stream raw data, this firmware acts as a **Smart Obstacle Detector**, filtering out background noise and specifically alerting only when objects are detected within a **50 cm safety radius**.

## Key Features
* **Custom Hardware:** Designed and fabricated a **double-layer (2-layer)** home-made PCB using the toner transfer method.
* **Step-by-Step Scanning:** Implements a stable 500ms delay between servo steps to eliminate mechanical jitter and ensure sensor settling.
* **Smart Filtering:** The system ignores distant objects and triggers a UART alert only for obstacles closer than **50 cm**.
* **Jitter-Free Logic:** Uses a hybrid timing approach (Timer Interrupts for Servo pacing + Precision Polling for Echo) to prevent hardware timer conflicts.

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
| **P1.0** | **Trig Output** | Generates 10µs trigger pulse for HC-SR04. |
| **P1.1** | **Echo Input** | Reads the pulse width from the sensor. |
| **P1.2** | **UART TX** | Transmits alert data to PC (9600 Baud). |
| **P1.6** | **Servo PWM** | Controls servo angle (Timer0_A1 Output). |

## How It Works
1.  **Positioning:** The MCU moves the servo to a specific angle (0° to 180° range with 10° steps).
2.  **Stabilization:** The system waits for **500ms** to allow the servo to reach the position and stop vibrating.
3.  **Measurement:** A trigger pulse is sent. The firmware measures the "Time of Flight" of the Echo signal using a calibrated polling loop to avoid interrupting the Servo's PWM signal.
4.  **Detection & Alert:**
    * If `Distance < 50 cm`: The system sends an **ALERT** string via UART (e.g., `ALERT: Angle 40 deg -> Distance 12 cm`).
    * If `Distance > 50 cm`: The system ignores the reading to reduce data noise.

## Fabrication Note
This project was designed and fabricated in-house as a rapid prototyping study to demonstrate **double-layer PCB fabrication techniques** and embedded system integration without external libraries.
