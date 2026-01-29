# MSP430 PWM Duty Cycle Meter

## Overview
This project utilizes the **MSP430G2553** microcontroller to measure the **Duty Cycle (%)** of an incoming PWM signal using the **Timer A Input Capture** module. The calculated value is transmitted to a PC via UART for monitoring (e.g., Serial Plotter).

## Features
* **Input Capture:** Uses `Timer0_A` to detect rising and falling edges with precision.
* **Low-Level Implementation:** Written in bare-metal C using direct register access (`TACTL`, `TACCTL1`, `TA0IV`).
* **Real-time Monitoring:** Sends floating-point duty cycle values over UART.
* **Watchdog:** Disabled for continuous operation.

## Hardware Pinout
| Pin | Function | Description |
| :--- | :--- | :--- |
| **P1.1** | **PWM Input** | Connect the signal source here (Timer A Capture Input `CCI1A`). |
| **P1.2** | **UART TX** | Connect to RX pin of USB-TTL converter (sends data to PC). |
| **GND** | **Ground** | Common ground with the signal source. |

## Configuration
* **Clock:** 1 MHz (Calibrated DCO).
* **UART Baud Rate:** 9600 bps.
* **Timer Mode:** Continuous Mode (counting up to 0xFFFF).
* **Capture Mode:** Both Edges (Rising & Falling).

## How it Works
1.  The Timer is configured to trigger an interrupt on **both edges** of the signal at P1.1.
2.  **ISR (Interrupt Service Routine)** captures the timer counter value:
    * Edge 1: Starts measurement.
    * Edge 2: Calculates pulse width (On-time).
    * Edge 3: Calculates total period.
3.  The main loop computes the percentage `(OnTime / Period) * 100` and transmits it via UART if a valid reading is available.
