/*
 * FRAMpen.h
 *
 *     Project: FRAMpen
 *  Created on: 14.01.2012
 *      Author: Max Groening
 *              Thomas Bendel
 */

#ifndef FRAMPEN_H_
#define FRAMPEN_H_


/**
 * Defines
 */

/* Accelerometer */
#define ACC_X_PIN         BIT0
#define ACC_Y_PIN         BIT1
#define ACC_Z_PIN         BIT2

#define ACC_X_CHANNEL     ADC10INCH_12
#define ACC_Y_CHANNEL     ADC10INCH_13
#define ACC_Z_CHANNEL     ADC10INCH_14

/* LEDs */
#define LED1_OUT          PJOUT
#define LED1_PIN          BIT0
#define LED2_OUT          PJOUT
#define LED2_PIN          BIT1
#define LED_BOTH          (LED1_PIN + LED2_PIN)

/* Time periods */
#define TIME_3SEC         12288
#define TIME_1SEC         4096
#define TIME_05SEC        2048
#define TIME_ON           128

#define TIME_SHORT_PUSH   250   // In msec
#define TIME_LONG_PUSH    2000  //

// The FRAM section from 0xD000 - 0xF000 is used
// for storing the acceleration data.
// Do not use this section for code or data placement.
// It will get overwritten!
#define ADC_START_ADDR    0xD400
#define ADC_END_ADDR      0xF000

// States
enum states {
	IDLE, RECORD, TRANSMIT
};

// Push button states
enum button_states {
	NO_PUSH, SHORT_PUSH, LONG_PUSH
};

extern volatile unsigned int *fram_ptr;  // Pointer to FRAM data block
extern volatile unsigned int adc_result;  // Stores intermediate ADC results
extern volatile unsigned int adc_cnt;  // ADC conversion counter
extern volatile unsigned int led_select;  // Select LED(s) to toggle
extern volatile unsigned int led_speed;  // Store current LED off time
extern volatile unsigned int wdt_cnt;  // Count seconds till shutdown
extern volatile unsigned int button_flag;  // Push button flag for ISR


#endif /* FRAMPEN_H_ */
