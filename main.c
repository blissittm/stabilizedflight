#include <msp430.h>

void init_ADC12(void) {
    // Configure ADC12
    ADC12CTL0 = SHT0_2 | ADC12ON;       // Sampling time, ADC12 on
    ADC12CTL1 = SHP;                    // Use sampling timer
    ADC12MCTL0 = INCH_0;                // Channel A0 (P6.0)
    ADC12CTL0 |= ENC;                   // Enable ADC12
}

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;           // Stop watchdog timer
    FLL_CTL0 |= XCAP18PF;               // Enable crystal

    // Setup for Servo 1 (Timer A, P1.2)
    P1DIR |= BIT2;                      // Set P1.2 as output (TA1.1 output)
    P1SEL |= BIT2;                      // Select TA1.1 functionality on P1.2

    // Setup for Servo 2 (Timer B, P2.1)
    P2DIR |= BIT2;                      // Set P2.2 as output (TB1.1 output)
    P2SEL |= BIT2;                   // Select TB0.1 functionality on P2.1

    // Timer A configuration (Servo 1)
    TACCR0 = 650;                       // PWM period (Servo 1)
    TACCR1 = 618;                       // PWM duty cycle for Servo 1
    TACCTL1 = OUTMOD_7;                 // Reset/Set mode
    TACTL = TASSEL_1 + MC_1;            // ACLK, Up mode

    // Timer B configuration (Servo 2)
    TBCCR0 = 650;                      // PWM period (Servo 2)
    TBCCR1 = 618;                      // PWM duty cycle for Servo 2
    TBCCTL1 = OUTMOD_7;                // Reset/Set mode
    TBCTL = TASSEL_1 + MC_1;           // ACLK, Up mode

    int adcValue = 1527;                // Initialize ADC value at center
    int LLim4 = 285;
    int LLim3 = 534;
    int LLim2 = 782;
    int LLim1 = 1030;
    int CLLim = 1445;
    int CHLim = 1608;
    int HLim1 = 2023;
    int HLim2 = 2271;
    int HLim3 = 2520;
    int HLim4 = 2768;

    // Set P6.0 as ADC input (A0 channel)
    P6SEL |= BIT0;
    init_ADC12();

    // Wait for the clock to stabilize
    do {
        IFG1 &= ~OFIFG;
    } while (IFG1 & OFIFG);

    while (1) {
        // Start ADC conversion
        ADC12CTL0 |= ADC12SC;
        while (ADC12CTL1 & ADC12BUSY);  // Wait for ADC conversion to complete
        adcValue = ADC12MEM0;           // Store ADC result

        // Servo 1 (Timer A) control
        if (adcValue < LLim4) {
            TACCR1 = 618;               // Max + for servo 1
            TBCCR1 = 585;              // Min - for servo 2
        } else if (adcValue > LLim3 && adcValue <= LLim4) {
            TACCR1 = 614;
            TBCCR1 = 588;
        } else if (adcValue > LLim2 && adcValue <= LLim3) {
            TACCR1 = 611;
            TBCCR1 = 592;
        } else if (adcValue > LLim1 && adcValue <= LLim2) {
            TACCR1 = 608;
            TBCCR1 = 595;
        } else if (adcValue > CLLim && adcValue <= LLim1) {
            TACCR1 = 605;
            TBCCR1 = 598;
        } else if (adcValue > CHLim && adcValue <= CLLim) {   // Center position
            TACCR1 = 601;
            TBCCR1 = 601;
        } else if (adcValue > HLim1 && adcValue <= CHLim) {
            TACCR1 = 598;
            TBCCR1 = 605;
        } else if (adcValue > HLim1 && adcValue <= HLim2) {
            TACCR1 = 595;
            TBCCR1 = 608;
        } else if (adcValue > HLim2 && adcValue <= HLim3) {
            TACCR1 = 592;
            TBCCR1 = 611;
        } else if (adcValue > HLim3 && adcValue <= HLim4) {
            TACCR1 = 588;
            TBCCR1 = 614;
        } else if (adcValue > HLim4) {
            TACCR1 = 585;               // Min - for servo 1
            TBCCR1 = 618;              // Max + for servo 2
        }
    }
}
