/*
 * FRAMpen.c
 *
 *     Project: FRAMpen
 *  Created on: 15.01.2012
 *      Author: Max Groening
 *              Thomas Bendel
 */


volatile unsigned int *fram_ptr = 0;  // Pointer to FRAM data block
volatile unsigned int adc_result = 0;  // Stores intermediate ADC results
volatile unsigned int adc_cnt = 0;  // ADC conversion counter
volatile unsigned int led_select = 0;  // Select LED(s) to toggle
volatile unsigned int led_speed = 0;  // Store current LED off time
volatile unsigned int wdt_cnt = 0;  // Count seconds till shutdown
volatile unsigned int button_flag = 0;  // Push button flag for ISR
