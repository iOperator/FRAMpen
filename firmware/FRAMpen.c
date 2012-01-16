/*
 * FRAMpen.c
 *
 *     Project: FRAMpen
 *  Created on: 15.01.2012
 *      Author: Max Groening
 *              Thomas Bendel
 */

#include "stdint.h"


volatile unsigned int *fram_ptr = 0;  // Pointer to FRAM data block
volatile unsigned int adc_result = 0;  // Stores intermediate ADC results
volatile uint8_t led_select = 0;  // Select LED(s) to toggle
volatile unsigned int wdt_cnt = 0;  // Count seconds till shutdown
volatile unsigned int button_flag = 0;  // Push button flag for ISR
volatile unsigned int adc_cnt = 0;  // ADC conversion counter
