#include <xc.h>

#include "canlib.h"
#include "timer.h"

#define STATUS_CHECK_PERIOD 500

// memory pool for the CAN tx buffer
uint8_t tx_pool[400];

static void can_msg_handler(const can_msg_t *msg) {
    if ((get_board_type_unique_id(msg) == BOARD_TYPE_UNIQUE_ID) &&
        (get_board_inst_unique_id(msg) == BOARD_INST_UNIQUE_ID)) {
        return;
    }

    switch (get_message_type(msg)) {
        case MSG_LEDS_ON:
            // Turn on all LED
            break;
        case MSG_LEDS_OFF:
            // Turn off all LED
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

    for (;;) {
        // CLRWDT();

        if ((millis() - last_status_millis) > STATUS_CHECK_PERIOD) {
            can_msg_t board_stat_msg;
            build_general_board_status_msg(PRIO_MEDIUM, millis(), 0, 0, &board_stat_msg);
            txb_enqueue(&board_stat_msg);
        }

        txb_heartbeat();
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
