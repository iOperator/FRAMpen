/*
 * vectors.c
 *
 *     Project: FRAMpen
 *  Created on: 15.01.2012
 *      Author: Max Groening
 *              Thomas Bendel
 */

#include "msp430fr5739.h"
#include "FRAMpen.h"
#include "subroutines.h"


// TimerA interrupt service routine
#pragma vector = TIMER0_A0_VECTOR
__interrupt void TIMER0_A0_ISR(void) {
	TA0CCTL0 &= ~CCIFG;  // Reset interrupt flag
	switch(led_select) {
		case LED1_PIN:
			if (!(LED1_OUT & LED1_PIN)) {  // LED is off
				TA0CTL |= MC_0 + TACLR;
				TA0CCR0 = TIME_ON;
				TA0CTL |= MC_1;
				LED1_OUT |= (LED1_PIN);
			} else {  // LED is on
				TA0CTL |= MC_0 + TACLR;
				TA0CCR0 = led_speed;
				TA0CTL |= MC_1;
				LED1_OUT &= ~(LED1_PIN);
			}
			break;
		case LED2_PIN:
			if (!(LED2_OUT & LED2_PIN)) {  // LED is off
				TA0CTL |= MC_0 + TACLR;
				TA0CCR0 = TIME_ON;
				TA0CTL |= MC_1;
				LED2_OUT |= (LED2_PIN);
			} else {  // LED is on
				TA0CTL |= MC_0 + TACLR;
				TA0CCR0 = led_speed;
				TA0CTL |= MC_1;
				LED2_OUT &= ~(LED2_PIN);
			}
			break;
		case LED_BOTH:
			LED1_OUT ^= LED1_PIN;
			LED2_OUT ^= LED2_PIN;
			break;
		default:
			break;
	}
}

// ADC interrupt service routine
#pragma vector = ADC10_VECTOR
__interrupt void ADC10_ISR(void) {
	switch(__even_in_range(ADC10IV,ADC10IV_ADC10IFG)) {
		case ADC10IV_NONE: break;               // No interrupt
		case ADC10IV_ADC10OVIFG: break;         // conversion result overflow
		case ADC10IV_ADC10TOVIFG: break;        // conversion time overflow
		case ADC10IV_ADC10HIIFG: break;         // ADC10HI
		case ADC10IV_ADC10LOIFG: break;         // ADC10LO
		case ADC10IV_ADC10INIFG: break;         // ADC10IN
		case ADC10IV_ADC10IFG:
			adc_result += ADC10MEM0;
			// *fram_ptr = adc_result;
			// fram_ptr++;
			ADC10CTL0 &= ~ADC10ENC;             // Stop conversion
			__bic_SR_register_on_exit(CPUOFF);  // Clear CPUOFF bit from 0(SR)
			break;
		default: break;
	}
}

// Watchdog timer interrupt service routine
#pragma vector = WDT_VECTOR
__interrupt void WDT_ISR(void) {
	SFRIFG1 &= ~(WDTIFG);  // Clear interrupt flag
	wdt_cnt++;             // Increment WDT counter to count seconds

	if(wdt_cnt > 20) {
		LED2_OUT &= ~(LED2_PIN);          // Make sure that LED1 is off
		__enable_interrupt();             // Ensure that interrupts are enabled
		// PMMCTL0 |= PMMPW + PMMREGOFF;  // Unlock PMM register and set flag to enter LPM4.5 with LPM4 request
		PMMCTL0_H = PMMPW_H;              //
		PMMCTL0_L |= PMMREGOFF;           //
		LPM4;  // Now enter LPM4.5
	} else if(wdt_cnt > 15) {
		LED1_OUT &= ~(LED1_PIN);  // Make sure that LED1 is off
		LED2_OUT ^= LED2_PIN;     // Toggle LED2
	} else if(wdt_cnt > 10) {
		toggle_led_off();
		LED1_OUT ^= LED1_PIN;     // Toggle LED1
    }
}

// Port 4 interrupt service routine
#pragma vector = PORT4_VECTOR
__interrupt void Port4InterruptHandler(void) {
	switch (__even_in_range(P4IV,P4IV_P4IFG7)) {
		case P4IV_NONE:
			break;
	    case P4IV_P4IFG0:
	    	PAIE &= ~BIT0;  // Disable interrupt for push button
	    	wdt_cnt = 0;  // Reset watchdog seconds counter on key press
	    	button_flag = 1;
	    	LPM4_EXIT;  // Stay in active mode after ISR
	    	break;
	    case P4IV_P4IFG1:
	    	break;
	    case P4IV_P4IFG2:
	    	break;
	    case P4IV_P4IFG3:
	    	break;
	    case P4IV_P4IFG4:
	    	break;
	    case P4IV_P4IFG5:
	    	break;
	    case P4IV_P4IFG6:
	    	break;
	    case P4IV_P4IFG7:
	    	break;
	}
}
