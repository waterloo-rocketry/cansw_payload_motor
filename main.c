#include <xc.h>

#include "canlib.h"
#include "timer.h"
#include "pwm.h"

#define STATUS_CHECK_PERIOD 500

// declare I/O pins
#define LS_LEFT     PORTBbits.RB1
#define LS_RIGHT    PORTBbits.RB2

#define LED_GREEN   LATAbits.LA1
#define LED_BLUE    LATAbits.LA0
#define LED_RED     LATAbits.LA2
#define LED_ON      0
#define LED_OFF     1

// declare servo constants
#define SERVO_PWR       LATBbits.LATB5
#define SERVO_IN        PORTBbits.RB3
#define SERVO_STOP_PWM  1500            // microseconds    
#define SERVO_RIGHT_PWM 700             // microseconds
#define SERVO_LEFT_PWM  2300            // microseconds
#define PWM_PERIOD      1000            // change later, place holder value

#pragma config LVP = ON


// memory pool for the CAN tx buffer
uint8_t tx_pool[400];

static void can_msg_handler(const can_msg_t *msg);
void init_pins(void);
void set_motor_pwm(uint16_t duty);

int main(void) {
    // SYSTEM_Initialize();

    // Set up CAN TX
    TRISC1 = 0;
    RC1PPS = 0x33;

    // Set up CAN RX
    TRISC0 = 1;
    ANSELC0 = 0;
    CANRXPPS = 0b00010000;

    // Set up CAN module
    can_timing_t can_setup;
    // can_generate_timing_params(_XTAL_FREQ, &can_setup);
    can_init(&can_setup, can_msg_handler);

    // set up CAN tx buffer
    txb_init(tx_pool, sizeof(tx_pool), can_send, can_send_rdy);

    // Enable global interrupts
    INTCON0bits.GIE = 1;

    // Set up timer 0 for millis
    timer0_init();

    uint32_t last_status_millis = millis();
    
    
    init_pins();
    
    // turn off LEDs for now
    LED_GREEN = LED_OFF;
    LED_BLUE = LED_OFF;
    LED_RED = LED_OFF;
    
    // initialize pwm
    // pwm_init(PWM_PERIOD);
    // pwm_enable();
    
    // start moving servo motor
    SERVO_PWR = 1;
//    set_motor_pwm(SERVO_LEFT_PWM);

    // forever loop
    for (;;) {
        
        
        // CLRWDT();
         
        // periodically send board status
        if ((millis() - last_status_millis) > STATUS_CHECK_PERIOD) {
            can_msg_t board_stat_msg;
            build_general_board_status_msg(PRIO_MEDIUM, millis(), 0, 0, &board_stat_msg);
            txb_enqueue(&board_stat_msg);
        }

        txb_heartbeat();
        
        if (LS_LEFT) {
            LED_BLUE = LED_ON;
        } else {
            LED_BLUE = LED_OFF;
        }
        
        if (LS_RIGHT) {
            LED_RED = LED_ON;
        } else {
            LED_RED = LED_OFF;
        }
        
    }
}

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
    
    // turn on status LED to signal successful startup
    LED_GREEN = LED_ON;
}

void set_motor_pwm(uint16_t duty) {
    // pwm_set_duty_cycle(duty);
}

static void can_msg_handler(const can_msg_t *msg) {
    if ((get_board_type_unique_id(msg) == BOARD_TYPE_UNIQUE_ID) &&
        (get_board_inst_unique_id(msg) == BOARD_INST_UNIQUE_ID)) {
        return;
    }

    switch (get_message_type(msg)) {
        case MSG_LEDS_ON:
            // Turn on all LED
            LED_GREEN = LED_ON;
            LED_BLUE = LED_ON;
            LED_RED = LED_ON;
            break;
        case MSG_LEDS_OFF:
            // Turn off all LED
            LED_GREEN = LED_OFF;
            LED_BLUE = LED_OFF;
            LED_RED = LED_OFF;
            break;
        case MSG_RESET_CMD:
            if (check_board_need_reset(msg)) {
                RESET();
            }
            break;
        default:
            break;
    }
}

// Remove line below once generate code with MCC
#pragma config MVECEN = OFF

static void __interrupt() interrupt_handler(void) {
    if (PIR5) {
        can_handle_interrupt();
    }

    if (PIE3bits.TMR0IE == 1 && PIR3bits.TMR0IF == 1) {
        timer0_handle_interrupt();
        PIR3bits.TMR0IF = 0;
    }
}
