/* 
 * File:   platform.h
 * Author: cstubbs
 *
 * Created on August 2, 2025, 10:16 PM
 */
#ifndef PLATFORM_H
#define	PLATFORM_H

// I/O pins
#define LS_LEFT PORTBbits.RB1
#define LS_RIGHT PORTBbits.RB2
#define LED_GREEN LATA1
#define LED_BLUE LATA0
#define LED_RED LATA2
#define SERVO_PWR LATB5
#define SERVO_IN PORTBbits.RB3

#define LED_ON      0
#define LED_OFF     1

void init_pins();
void toggle_blue_led();
void toggle_green_led(); 
void toggle_red_led();

#endif

