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
#define LED_GREEN LATAbits.LA1
#define LED_BLUE LATAbits.LA0
#define LED_RED LATAbits.LA2
#define SERVO_PWR LATBbits.LATB5
#define SERVO_IN PORTBbits.RB3

#define LED_ON      0
#define LED_OFF     1

void init_pins();

#endif

