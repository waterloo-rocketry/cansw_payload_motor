#include <xc.h>
#include "platform.h"

void init_pins(void) {
    // setup LED pins
    TRISA0 = 0;
    TRISA1 = 0;
    TRISA2 = 0;
    
    ANSELA0 = 0;
    ANSELA1 = 0;
    ANSELA2 = 0;
    
    LED_GREEN = LED_OFF;
    LED_BLUE = LED_OFF;
    LED_RED = LED_OFF;
    
    // setup limit switch pins
    TRISB1 = 1;
    TRISB2 = 1;
    
    ANSELB1 = 0;
    ANSELB2 = 0;
    
    // setup servo power pin
    TRISB5 = 0;
    
    // setup servo pwm pin
    TRISB3 = 0;
    ANSELB3 = 0;
    
    // Set up CAN TX
    TRISC1 = 0;
    RC1PPS = 0x33;

    // Set up CAN RX
    TRISC0 = 1;
    ANSELC0 = 0;
    CANRXPPS = 0b00010000;
}

void toggle_blue_led() {
    LED_BLUE ^= 1;
}

void toggle_green_led() {
    LED_GREEN ^= 1;
}

void toggle_red_led() {
    LED_RED ^= 1;
}