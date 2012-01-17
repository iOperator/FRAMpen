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

	__enable_interrupt();

	while(1) {

		/* Check push button */
		if (button_flag == 1) {
			button_flag = 0;

			// Check push button
			unsigned int push_cnt = 0;
			button_state = NO_PUSH;
			__delay_cycles(200000);  // Wait 25 msec to debounce
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
			__delay_cycles(400000);  // Wait 50 msec to debounce
			__delay_cycles(400000);  // Wait 50 msec to debounce
		}

		LED1_off();
		LED2_off();


		/* Check states */
		switch (state) {
			case IDLE:
//				toggle_led(LED1_PIN, TIME_3SEC);
				switch (button_state) {
					case LONG_PUSH:
						state = TRANSMIT;
						wdt_disable();
						toggle_led(LED2_PIN, TIME_05SEC);
						LED2_off();
						transmit_data();
						break;
					case SHORT_PUSH:
						state = RECORD;
						wdt_disable();
						toggle_led(LED1_PIN, TIME_1SEC);
						LED1_off();
						record_data();
						break;
					default:
						break;
				}
				break;

			case RECORD:
				switch (button_state) {
					case LONG_PUSH:  // Delete data
						delete_data();
						state = IDLE;
						break;
					case SHORT_PUSH:  // Stop recording
						// Recording stop from inside recorde()
						state = IDLE;
						break;
					default:
						break;
				}
//				state = IDLE;
				break;
			case TRANSMIT:
				switch (button_state) {
					case LONG_PUSH:
						break;
					case SHORT_PUSH:
						break;
					default:
						break;
				}
				state = IDLE;
				break;
			default:
				break;
		}

		//button_state = NO_PUSH;
		PAIE |= BIT0;  // Enable interrupt for push button

		if (state == IDLE) {
			toggle_led(LED1_PIN, TIME_3SEC);
			wdt_enable();  // Activate auto-deep-sleep-watchdog
			LPM3;
			_nop();
		}

	}

}
