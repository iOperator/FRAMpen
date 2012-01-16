/*
 * main.c
 *
 *     Project: FRAMpen
 *  Created on: 14.01.2012
 *      Author: Max Groening
 *              Thomas Bendel
 */

#include "msp430fr5739.h"
#include "FRAMpen.h"
#include "init.h"
#include "vectors.h"
#include "subroutines.h"


void main(void) {

	init_system();

	volatile enum states state;
	state = IDLE;
	volatile enum button_states button_state;
	button_state = NO_PUSH;

//	PMMCTL0_H = PMMPW_H;  // open PMM
//	PM5CTL0 &= ~LOCKLPM5;  // Clear LOCKIO and enable ports
//	PMMCTL0_H = 0x00;  // close PMM

//	P4IFG = 0;  // P4 IFG cleared

	// wdt_enable();

	// LED1_on();


	while(1){

		__enable_interrupt();
		LPM3;
		_nop();

		/* Check push button */
		if (button_flag == 1) {
			button_flag = 0;

			// Check push button
			unsigned int push_cnt = 0;
			button_state = NO_PUSH;
			__delay_cycles(200000);  // Wait 25 msec to debounce
			while (!(P4IN & BIT0)) {  // Button pushed
				__delay_cycles(8000);  // Wait 1 msec
				push_cnt++;

				if (push_cnt > TIME_SHORT_PUSH) {  // Push button was pushed a short time
					button_state = SHORT_PUSH;
					LED1_on();
				}
				if (push_cnt > TIME_LONG_PUSH) {  // Push button was pushed a long time
					button_state = LONG_PUSH;
					LED2_on();
				}
			}
		}

		LED1_off();
		LED2_off();


		/* Check states */
		switch (state) {
			case IDLE:
				toggle_led(LED1_PIN, TIME_3SEC);
				switch (button_state) {
					case LONG_PUSH:
//						state = TRANSMIT;
						transmit_data();
						break;
					case SHORT_PUSH:
//						state = RECORD;
						toggle_led_off();
						record_data();
						break;
					default:
						break;
				}
				break;

			case RECORD:
				toggle_led(LED1_PIN, TIME_1SEC);
				switch (button_state) {
					case LONG_PUSH:
						break;
					case SHORT_PUSH:
						// Stop recording

						break;
					default:
						break;
				}
				break;
			case TRANSMIT:
				toggle_led(LED1_PIN, TIME_05SEC);
				switch (button_state) {
					case LONG_PUSH:
						break;
					case SHORT_PUSH:
						break;
					default:
						break;
				}
				break;
			default:
				break;
		}

		button_state = NO_PUSH;

	}

}
