/*
 * subroutines.c
 *
 *     Project: FRAMpen
 *  Created on: 15.01.2012
 *      Author: Max Groening
 *              Thomas Bendel
 */

#include "stdint.h"
#include "msp430fr5739.h"
#include "FRAMpen.h"
#include "init.h"


/**
 * Enable accelerometer
 */
void acc_power_up() {
	P2OUT |= BIT7;  // Enable ACC
	// Allow the accelerometer to settle before sampling any data
	__delay_cycles(200000);
}

/**
 * Disable accelerometer
 */
void acc_power_down() {
	P2OUT &= ~(BIT7);
}

/**
 * Enable UART interrupt
 */
void uart_ien() {
	UCA0IE |= UCRXIE;  // enable RX interrupt
}

/**
 * Disable UART interrupt
 */
void uart_ied() {
	UCA0IE &= ~(UCRXIE);  // disable RX interrupt
}

/**
 * Enable LED 1
 */
void LED1_on() {
	LED1_OUT |= LED1_PIN;
}

/**
 * Disable LED 1
 */
void LED1_off() {
	LED1_OUT &= ~LED1_PIN;
}

/**
 * Enable LED 2
 */
void LED2_on() {
	LED2_OUT |= LED2_PIN;
}

/**
 * Disable LED 2
 */
void LED2_off() {
	LED2_OUT &= ~LED2_PIN;
}

/**
 * Blink LED(s)
 */
void toggle_led(unsigned int led, unsigned int speed)
{
	led_select = led;
	led_speed = speed;
	timer_a_init(led_speed);
}

/**
 * Stop toggle LED(s)
 */
void toggle_led_off() {
	TA0CTL = MC_0 + TACLR;  // Stop timer and clear TAxR
}

/**
 * Enable watchdog with 1 sec interval timer
 */
void wdt_enable()
{
	WDTCTL = WDTPW + WDTHOLD;  // Stop watchdog for initialization
	// Select ACLK, interval timer mode, clear watchdog timer counter, interval 1 sec
	WDTCTL = WDTPW + WDTSSEL_1 + WDTTMSEL + WDTCNTCL + WDTIS_4;

    SFRIE1 |= WDTIE;  // Enable WDT interrupt
}

/**
 * Hold watchdog timer
 */
void wdt_disable() {
	WDTCTL = WDTPW + WDTHOLD + WDTCNTCL;  // Hold watchdog timer
	wdt_cnt = 0;  // Reset watchdog seconds counter
}

/**
 * Sample coordinates to FRAM
 */
void record_data() {
	acc_power_up();

	init_adc();

	fram_ptr = (unsigned int *)ADC_START_ADDR;

	while(fram_ptr < (unsigned int *)(ADC_END_ADDR - 3)) {
		if (button_flag == 1) {
			break;
		}
		_nop();
		adc_result = 0;  // Clear intermediate results
		for (adc_cnt = 0; adc_cnt < 16; ++adc_cnt) {  // Sample 16 times
			ADC10MCTL0 = ADC10SREF_0 + ACC_X_CHANNEL;
			while (ADC10CTL1 & BUSY);
			ADC10CTL0 |= ADC10ENC + ADC10SC;  // Start conversion
			__bis_SR_register(CPUOFF + GIE);  // LPM0, ADC10_ISR will force exit
//			__no_operation();
		}
		*fram_ptr = adc_result;
		fram_ptr++;
		adc_result = 0;  // Clear intermediate results
		for (adc_cnt = 0; adc_cnt < 16; ++adc_cnt) {  // Sample 16 times
			ADC10MCTL0 = ADC10SREF_0 + ACC_Y_CHANNEL;
			while (ADC10CTL1 & BUSY);
			ADC10CTL0 |= ADC10ENC + ADC10SC;  // Start conversion
			__bis_SR_register(CPUOFF + GIE);  // LPM0, ADC10_ISR will force exit
//			__no_operation();
		}
		*fram_ptr = adc_result;
		fram_ptr++;
		adc_result = 0;  // Clear intermediate results
		for (adc_cnt = 0; adc_cnt < 16; ++adc_cnt) {  // Sample 16 times
			ADC10MCTL0 = ADC10SREF_0 + ACC_Z_CHANNEL;
			while (ADC10CTL1 & BUSY);
			ADC10CTL0 |= ADC10ENC + ADC10SC;  // Start conversion
			__bis_SR_register(CPUOFF + GIE);  // LPM0, ADC10_ISR will force exit
//			__no_operation();
		}
		*fram_ptr = adc_result;
		fram_ptr++;
	}  // End while

	// Done recording
	toggle_led_off();
	LED1_on();
	LED2_on();
	__delay_cycles(8000000);
	__delay_cycles(8000000);
	LED1_off();
	LED2_off();
}

/**
 * Transmit coordinate data via UART
 */
void transmit_data() {
	init_uart();

	unsigned int *current_address_ptr = (unsigned int *)ADC_START_ADDR;

	while(current_address_ptr < (unsigned int *)ADC_END_ADDR) {
		while (UCBUSY & UCA0STATW) {};            // Wait while UART is busy
		UCA0TXBUF = (*current_address_ptr) >> 8;  // Send high byte first
		while (UCBUSY & UCA0STATW) {};            // Wait while UART is busy
		UCA0TXBUF = *current_address_ptr;         // Send low byte
		current_address_ptr++;
	}
}

/**
 * Deletes all coordinate data stored in FRAM
 */
void delete_data() {
	unsigned int *current_address_ptr = (unsigned int *)ADC_START_ADDR;

	while(current_address_ptr < (unsigned int *)ADC_END_ADDR) {
		*current_address_ptr = 0xFFFF;
		current_address_ptr++;
	}
	// Done deleting
	unsigned int i = 0;
	toggle_led_off();
	for (i = 0; i < 5; i++) {
		LED1_on();
		LED2_on();
		__delay_cycles(1000000);
		LED1_off();
		LED2_off();
		__delay_cycles(1000000);
	}
}
