#include <xc.h>

#include "mcc_generated_files/system/system.h"

#include "canlib.h"
#include "timer.h"
#include "pwm.h"
#include "platform.h"

#define STATUS_CHECK_PERIOD 500
#define LIM_PERIOD 250

#pragma config LVP = ON

// memory pool for the CAN tx buffer
uint8_t tx_pool[400];

static void can_msg_handler(const can_msg_t *msg);

uint8_t pwm_period = 1;

int main(void) {
    SYSTEM_Initialize();
    
    init_pins();

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
    uint32_t last_mtr_pulse = millis();
    uint32_t last_lim_millis = millis();
    
    // just for tests this should be can message
    SERVO_PWR = 1;

    // forever loop
    for (;;) {
        // CLRWDT();
         
        // periodically send board status
        if ((millis() - last_status_millis) > STATUS_CHECK_PERIOD) {
            can_msg_t board_stat_msg;
            build_general_board_status_msg(PRIO_MEDIUM, millis(), 0, 0, &board_stat_msg);
            txb_enqueue(&board_stat_msg);
            
            toggle_blue_led();
        }
        
        if (SERVO_PWR && (millis() - last_mtr_pulse) > pwm_period) {
            SERVO_IN ^= 1;
        }
        
        // limit switch detection
        if (LS_LEFT) {
            LED_GREEN = LED_ON;
            pwm_period = 1;
            
            // dont want to spam can messages before it reverses
            if ((millis() - last_lim_millis() > LIM_PERIOD)){
                can_msg_t lim_stat_msg;
                build_analog_data_msg(PRIO_MEDIUM, millis(), SENSOR_ENUM_MAX, 1, &lim_stat_msg);
                txb_enqueue(&lim_stat_msg);
            }
        } else {
            LED_GREEN = LED_OFF;
        }
        if (LS_RIGHT) {
            LED_RED = LED_ON;
            pwm_period = 2;
            
            if ((millis() - last_lim_millis() > LIM_PERIOD)){
                can_msg_t lim_stat_msg;
                build_analog_data_msg(PRIO_MEDIUM, millis(), SENSOR_ENUM_MAX, 2, &lim_stat_msg);
                txb_enqueue(&lim_stat_msg);
            }
        } else {
            LED_RED = LED_OFF;
        }
        
        txb_heartbeat();
        
    }
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
        case MSG_ACTUATOR_CMD:
            SERVO_PWR ^= 1;
        default:
            break;
    }
}

static void __interrupt() interrupt_handler(void) {
    if (PIR5) {
        can_handle_interrupt();
    }

    if (PIE3bits.TMR0IE == 1 && PIR3bits.TMR0IF == 1) {
        timer0_handle_interrupt();
        PIR3bits.TMR0IF = 0;
    }
}