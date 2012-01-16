/*
 * init.c
 *
 *     Project: FRAMpen
 *  Created on: 14.01.2012
 *      Author: Max Groening
 *              Thomas Bendel
 */

#include "stdint.h"
#include "msp430fr5739.h"


/**
 *  Setup WDT, CS and ports
 */
void init_system() {

    /* Watchdog timer */
    WDTCTL = WDTPW + WDTHOLD;  // Stop WDT


    /********************************************************************/
    /* Set all unused ports as outputs to prevent floating inputs.      */
    /*                                                                  */
    /* P2.0  F/S  UCA0TXD                                               */
    /* P2.1  F/S  UCA0RXD                                               */
    /* P2.7  OUT  Accelerometer power                                   */
    /*                                                                  */
    /* P3.0  IN   Accelerometer X axis                                  */
    /* P3.1  IN   Accelerometer Y axis                                  */
    /* P3.2  IN   Accelerometer Z axis                                  */
    /*                                                                  */
    /* P4.0  IN   Push button                                           */
    /*                                                                  */
    /* PJ.0  OUT  LED 1                                                 */
    /* PJ.1  OUT  LED 2                                                 */
    /* PJ.4  F/S  XIN                                                   */
    /* PJ.5  F/S  XOUT                                                  */
    /*                                                                  */
    /********************************************************************/

    /* Port 1 */
    P1OUT &= ~(BIT0 + BIT1 + BIT2 + BIT3 + BIT5 + BIT6 + BIT7);  // Output '0'
    P1DIR |= (BIT0 + BIT1 + BIT2 + BIT3 + BIT5 + BIT6 + BIT7);  // Outputs

    /* Port 2 */
    P2OUT &= ~(BIT0 + BIT1 + BIT2 + BIT3 + BIT5 + BIT6 + BIT7);  // Output '0'
    P2DIR |= (BIT0 + BIT1 + BIT2 + BIT3 + BIT5 + BIT6 + BIT7);  // Outputs
    // Configure UART pins P2.0 & P2.1
    P2SEL1 |= BIT0 + BIT1;
    P2SEL0 &= ~(BIT0 + BIT1);

    /* Port 3 */
    // P3.0, P3.1 P3.2 as inputs (Accelerometer X-, Y-, and Z- axis)
    P3OUT &= ~(BIT0 + BIT1 + BIT2 + BIT3 + BIT5 + BIT6 + BIT7);  // Output '0'
    P3DIR |= (BIT3 + BIT5 + BIT6 + BIT7);  // Outputs
    P3REN |= (BIT0 + BIT1 + BIT2);  // Pull down

    /* Port 4 */
    // Push button on P4.0 as input
    P4OUT &= ~(BIT1 + BIT2 + BIT3 + BIT5 + BIT6 + BIT7);  // Output '0'
    P4OUT |= BIT0;
    P4DIR |= (BIT1 + BIT2 + BIT3 + BIT5 + BIT6 + BIT7);  // Outputs
    P4REN |= (BIT0);  // Pull up P4.0

    // P4IES &= ~(BIT0);  // P4.0 lo/hi edge interrupt
    P4IES |= BIT0;  // P4.0 hi/lo edge interrupt
    P4IE = BIT0;  // P4.0 interrupt enabled
    P4IFG = 0;  // P4 IFG cleared

    /* Port J */
    // LEDs on PJ.0 and PJ.1 are off
    PJOUT &= ~(BIT0 + BIT1 + BIT2 + BIT3 + BIT4 + BIT5);
    PJDIR |= (BIT0 + BIT1 + BIT2 + BIT3 + BIT4 + BIT5);
    // Enable XT1
    PJSEL1 &= ~(BIT4 + BIT5);
    PJSEL0 |= (BIT4 + BIT5);


    /* Clock system */
    // Startup clock system in max. DCO setting ~8MHz
    CSCTL0 = CSKEY;  // Unlock register
    CSCTL1 |= DCOFSEL0 + DCOFSEL1;  // Set max. DCO setting
    CSCTL2 = SELA_0 + SELS_3 + SELM_3;  // Set ACLK = XT1; SMCLK = DCO; MCLK = DCO
    CSCTL3 = DIVA_0 + DIVS_0 + DIVM_0;  // Set all dividers

    /* Wait for stable XT1 */
    do {
    	CSCTL5 &= ~XT1OFFG;  // Clear XT1 fault flag
        SFRIFG1 &= ~OFIFG ;
    } while (SFRIFG1 & OFIFG);  // Test oscillator fault flag


    // Turn off temperature sensor
    REFCTL0 |= REFTCOFF;
    REFCTL0 &= ~REFON;
}


/**
 * Setup ADC
 */
void init_adc() {

	// Single channel, single conversion
	ADC10CTL0 &= ~ADC10ENC;  // Ensure ENC is clear
	ADC10CTL0 = ADC10ON + ADC10SHT_12;  // S&H 1024 cycles, activate ADC
	ADC10CTL1 = ADC10SHS_0 + ADC10SHP + ADC10CONSEQ_0 + ADC10SSEL_2;  // ADC10SC bit, pulse mode, MCLK
	ADC10CTL2 = ADC10RES;  // 10 bit resolution
	ADC10MCTL0 = ADC10SREF_0 + ADC10INCH_12;  // AVCC--AVSS, channel A12
	ADC10IV = 0x00;  // Clear all ADC12 channel interrupt flags
	ADC10IE |= ADC10IE0;
}


/**
 * Setup UART0
 */
void init_uart() {

	UCA0CTL1 |= UCSWRST;  // eUSCI reset state
	UCA0CTL1 = UCSSEL_1;  // Set ACLK = 32768 as UCBRCLK
	UCA0BR0 = 3;          // Set prescaler of BRG for 9600 Baud
	UCA0MCTLW |= 0x9200;  // 32768/9600 - INT(32768/9600)=0.41
	UCA0BR1 = 0;          // UCBRSx value = 0x92 (See UG)
	UCA0CTL1 &= ~UCSWRST; // Release from reset
}

/**
 *  Setup TimerA
 */
void timer_a_init(uint16_t speed) {

	TA0CTL = MC_0 + TACLR;        // Stop timer and clear TAxR
	TA0CTL |= (TASSEL_1 + ID_3);  // Source ACLK, divider /8
	TA0CCTL0 = CCIE;              // Capture/compare interrupt enable
	TA0CCR0 = speed;              // Load TimerA compare register
	TA0CTL |= MC_1;               // Start timer in up mode (counts to TA0CCR0)
}
