/*
 * init.h
 *
 *     Project: FRAMpen
 *  Created on: 14.01.2012
 *      Author: Max Groening
 *              Thomas Bendel
 */

#ifndef INIT_H_
#define INIT_H_

#include "stdint.h"


/* Setup WDT, CS and ports */
void init_system();

/* Setup ADC */
void init_adc();

/* Setup UART */
void init_uart();

/* Setup TimerA */
void timer_a_init(unsigned int speed);


#endif /* INIT_H_ */
