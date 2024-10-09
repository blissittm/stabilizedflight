#include <msp430.h>

// Initialize ADC12 configuration
void init_ADC12(void) {
    ADC12CTL0 = SHT0_2 | ADC12ON;        // Sampling time, ADC12 on
    ADC12CTL1 = SHP;                     // Use sampling timer
    ADC12MCTL0 = INCH_0;                 // Channel A0 (P6.0)
    ADC12CTL0 |= ENC;                    // Enable ADC12
}

int main(void) {
    WDTCTL = WDTPW | WDTHOLD;            // Stop watchdog timer
    FLL_CTL0 |= XCAP18PF;                // Enable crystal for ACLK

    // Setup for Servo 1 (Timer A, P1.2)
    P1DIR |= BIT2;                       // Set P1.2 as output (TA1.1)
    P1SEL |= BIT2;                       // Select TA1.1 functionality on P1.2

    // Timer A configuration for Servo 1 (PWM)
    TACCR0 = 650;                        // PWM period
    TACCR1 = 618;                        // Initial PWM duty cycle
    TACCTL1 = OUTMOD_7;                  // Reset/Set mode
    TACTL = TASSEL_1 | MC_1;             // ACLK, Up mode

    int adcValue = 1527;                 // Initialize ADC value at midpoint
    float Value = adcValue - 285;        // Zero-adjusted ADC value
    float DCValue = 618 - 585;           // Initial duty cycle value

    // Configure P6.0 as ADC input (A0 channel)
    P6SEL |= BIT0;
    init_ADC12();                        // Initialize ADC12

    // Wait for ACLK to stabilize
    do {
        IFG1 &= ~OFIFG;                  // Clear oscillator fault flag
    } while (IFG1 & OFIFG);              // Wait for stabilization

    // Main loop
    while (1) {
        ADC12CTL0 |= ADC12SC;            // Start ADC conversion
        while (ADC12CTL1 & ADC12BUSY);   // Wait for conversion to complete
        adcValue = ADC12MEM0;            // Read ADC result

        // Servo control calculations
        Value = adcValue - 285;          // Adjust ADC result
        DCValue = (Value / 2483) * 33;   // Convert to duty cycle (0-33%)

        // Update PWM signal based on calculated DCValue
        if (DCValue < 0) {
            TACCR1 = 585;                // Set minimum PWM duty cycle
        } else if (DCValue > 33) {
            TACCR1 = 618;                // Set maximum PWM duty cycle
        } else {
            TACCR1 = (int)DCValue + 585; // Adjust PWM duty cycle based on ADC value
        }
    }
}

