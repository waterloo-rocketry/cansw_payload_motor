#include "platform.h"

void init_pins(void) {
    // setup LED pins
    TRISAbits.TRISA0 = 0;
    TRISAbits.TRISA1 = 0;
    TRISAbits.TRISA2 = 0;
    
    ANSELAbits.ANSELA0 = 0;
    ANSELAbits.ANSELA1 = 0;
    ANSELAbits.ANSELA2 = 0;
    
    LED_GREEN = LED_OFF;
    LED_BLUE = LED_OFF;
    LED_RED = LED_OFF;
    
    // setup limit switch pins
    TRISBbits.TRISB1 = 1;
    TRISBbits.TRISB2 = 1;
    
    ANSELBbits.ANSELB1 = 0;
    ANSELBbits.ANSELB2 = 0;
    
    // setup servo power pin
    TRISBbits.TRISB5 = 0;
    SERVO_PWR = 1;
    
    // setup servo pwm pin
    TRISBbits.TRISB3 = 0;
    ANSELBbits.ANSELB3 = 0;
    
    // Set up CAN TX
    TRISC1 = 0;
    RC1PPS = 0x33;

    // Set up CAN RX
    TRISC0 = 1;
    ANSELC0 = 0;
    CANRXPPS = 0b00010000;
}
