#include <msp430.h>
#include <stdio.h>

/*
 * Project: MSP430 Ultrasonic Object Detector (Radar)
 * Hardware: MSP430G2553 LaunchPad
 *
 * Description:
 * This system performs a step-by-step radar scan using a Servo Motor and
 * an HC-SR04 Ultrasonic Sensor.
 * - Scanning Range: 0 to 180 degrees.
 * - Detection Mode: Reports obstacles only if they are closer than 50 cm.
 * - Step Delay: Moves every 500ms for stable readings.
 *
 * Pin Configuration:
 * P1.0 -> HC-SR04 Trig (Output)
 * P1.1 -> HC-SR04 Echo (Input)
 * P1.2 -> UART TX (9600 Baud)
 * P1.6 -> Servo PWM Signal (Timer0_A1)
 */

#define TRIG_PIN  BIT0
#define ECHO_PIN  BIT1
#define TXD       BIT2
#define SERVO_PIN BIT6

// Global Variables
volatile int angle = 0;
volatile int direction = 1;
volatile int step_ready = 0;

// Blocking microsecond delay (approx. for 1MHz clock)
void delay_us(unsigned int us) {
    while (us--) __delay_cycles(1);
}

// UART Initialization
void init_UART(void) {
    P1SEL |= TXD;
    P1SEL2 |= TXD;
    UCA0CTL1 |= UCSSEL_2; // Use SMCLK
    UCA0BR0 = 104;        // 1MHz 9600 Baud
    UCA0BR1 = 0;
    UCA0MCTL = UCBRS0;    // Modulation
    UCA0CTL1 &= ~UCSWRST; // Initialize USCI state machine
}

// Send String via UART
void sendString_UART(char* str) {
    while (*str) {
        while (!(IFG2 & UCA0TXIFG)); // Wait for TX buffer
        UCA0TXBUF = *str++;
    }
}

// Set Servo Position
void set_servo_angle(int angle) {
    // Safety limits
    if (angle > 180) angle = 180;
    if (angle < 0) angle = 0;
    
    // PWM calculation for standard Micro Servos (SG90/MG90S)
    // Map 0-180 degrees to CCR values
    CCR1 = 600 + (angle * 10); 
}

// Measure Distance using HC-SR04
unsigned int measure_distance_cm(void) {
    unsigned int timeout = 30000;
    unsigned int duration_counter = 0;

    // Send 10us Trigger Pulse
    P1OUT |= TRIG_PIN;
    delay_us(10);
    P1OUT &= ~TRIG_PIN;

    // Wait for Echo HIGH
    while (!(P1IN & ECHO_PIN) && timeout--);
    if (timeout == 0) return 0; // Timeout error

    // Measure Pulse Width (Polling method to avoid Timer reset conflict)
    timeout = 30000;
    while ((P1IN & ECHO_PIN) && timeout--) {
        duration_counter++;
        __delay_cycles(10); // Resolution adjustment loop
    }

    // Calculate Distance in cm (Calibrated for 1MHz & loop overhead)
    return duration_counter / 4; 
}

// Initialize Timer A0 for PWM and Time Base
void init_timerA(void) {
    TACTL = TASSEL_2 + MC_1 + ID_0; // SMCLK, Up Mode
    
    // PWM Period: 20ms (Standard 50Hz for Servos)
    CCR0 = 20000 - 1; 
    CCTL0 = CCIE; // Enable Interrupt for time base logic

    // PWM Output Config (P1.6 / TA0.1)
    CCTL1 = OUTMOD_7; // Reset/Set Mode
    CCR1 = 1500;      // Set initial position to 90 degrees
}

// Timer A0 Interrupt Service Routine
// Handles timing for the scanning steps
#pragma vector = TIMER0_A0_VECTOR
__interrupt void TimerA_ISR(void) {
    static int tick = 0;

    tick++;
    // Step Delay Logic:
    // Interrupt triggers every 20ms.
    // 25 ticks * 20ms = 500ms delay between steps.
    if (tick >= 25) { 
        tick = 0;
        step_ready = 1; // Flag to execute next step in main loop
    }
}

void main(void) {
    // Stop Watchdog Timer
    WDTCTL = WDTPW | WDTHOLD;
    
    // Set Clock to 1MHz
    BCSCTL1 = CALBC1_1MHZ;
    DCOCTL = CALDCO_1MHZ;

    // GPIO Setup
    P1DIR |= TRIG_PIN + SERVO_PIN; // Outputs
    P1OUT &= ~TRIG_PIN;            // Low
    P1DIR &= ~ECHO_PIN;            // Input
    P1SEL |= SERVO_PIN;            // Select Timer function for P1.6
    P1SEL2 &= ~SERVO_PIN;

    init_UART();
    init_timerA();
    
    // Enable Global Interrupts
    __enable_interrupt();

    sendString_UART("System Initialized. Scanning for objects < 50cm...\r\n");

    char buffer[60];

    while (1) {
        // Execute logic only when the time step is reached (every 500ms)
        if (step_ready) {
            step_ready = 0; // Clear flag

            // 1. Update Servo Position
            set_servo_angle(angle);
            
            // Wait for mechanical settling
            __delay_cycles(50000); 

            // 2. Measure Distance
            unsigned int dist = measure_distance_cm();

            // 3. Object Detection Filter
            // Only report if a valid object is detected within 50 cm
            if (dist > 0 && dist < 50) {
                sprintf(buffer, "ALERT: Object Detected -> Angle: %d | Dist: %d cm\r\n", angle, dist);
                sendString_UART(buffer);
            }

            // 4. Update Scanning Angle (10-degree steps)
            angle += 10 * direction;

            // Reverse direction at limits (0 and 180 degrees)
            if (angle >= 180) {
                angle = 180;
                direction = -1;
            } else if (angle <= 0) {
                angle = 0;
                direction = 1;
            }
        }
    }
}