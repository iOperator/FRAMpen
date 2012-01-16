/*
 * subroutines.h
 *
 *     Project: FRAMpen
 *  Created on: 14.01.2012
 *      Author: Max Groening
 *              Thomas Bendel
 */

#ifndef SUBROUTINES_H_
#define SUBROUTINES_H_

/* Enable accelerometer */
void acc_power_up();

/* Disable accelerometer */
void acc_power_down();

/* Enable UART interrupt */
void uart_ien();

/* Disable UART interrupt */
void uart_ied();

/* Enable LED 1 */
void LED1_on();

/* Disable LED 1 */
void LED1_off();

/* Enable LED 2 */
void LED2_on();

/* Disable LED 2 */
void LED2_off();

/* Toggle LED(s) */
void toggle_led(unsigned int led, unsigned int speed);

/* Stop toggle LED(s) */
void toggle_led_off();

/* Enable Watchdog & Interrupt */
void wdt_enable();

/* Disable Watchdog */
void wdt_disable();

/* Sample coordinates to FRAM */
void record_data();

/* Transmit coordinate data via UART */
void transmit_data();

/* Delete data */
void delete_data();


#endif /* SUBROUTINES_H_ */
