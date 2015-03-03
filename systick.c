/*
 * systick.c
 */

#include "systick.h"
#include "fsm.h"

static uint32_t count = 0;
static uint32_t count_to_val;
static void(*callback_fn)();

extern uint32_t cmd;
extern uint32_t ping;
extern uint32_t adc;
extern uint32_t servo;
extern uint32_t prev_case;
extern uint32_t send_cmd_message;
//extern uint32_t send_exit_message;


void __attribute__ ((interrupt)) systick_handler(void)
{
	if(callback_fn) {
		callback_fn();
	}
}

void systick_init(uint32_t end_val) {

	callback_fn = systick_1sec_callback_fn;
	count_to_val = end_val; /* Timer end value */
	uint32_t x = 0;
	uint32_t y = 0x7;
	SYSTICK->CS = x;			/* Step 1 - Disable timer */
	SYSTICK->RV = WAIT_CYCLES;	/* Load # cycles to wait before interrupt */
	SYSTICK->CV = x;			/* Set current value to 0 */
	SYSTICK->CS = y;			/* Load 7 to start interrupts */
}

void systick_on(void) {
	/* Set current value to 0 */
	SYSTICK->CV = 0;
	/* Start 1 second interrupts */
	SYSTICK->CS = 0x7;
}

void systick_off(void) {
	/* Disable timer */
	SYSTICK->CS = 0;
}

void systick_1sec_callback_fn(void) {
	/* Timer counts in 1 sec intervals to desired value */
	count = count + 1;
	if(count >= count_to_val && fsm_lock() == FSM_LOCK_ACQUIRED) {
		systick_off();
		count = 0;
		switch(fsm_get_state()) {
		case UB_DEBOUNCE:
			if (prev_case == cmd) {
				fsm_set_state(PING_MODE1);
			}
			else if (prev_case == ping) {
				fsm_set_state(POT_UPDATE1);
			}
			else if (prev_case == adc) {
				fsm_set_state(SERVO_MODE1);
			}
			else if (prev_case == servo) {
				fsm_set_state(CMD_MODE1);
			}
			break;
		case CMD_MODE1:
			if(send_cmd_message == 0) {
				fsm_set_state(CMD_MODE2);
			}
			else {
				fsm_set_state(CMD_MODE1);
			}
			break;
		case PING_MODE1:
			fsm_set_state(PING_MODE2);
			break;
		case POT_UPDATE1:
			fsm_set_state(POT_UPDATE2);
			break;
		case SERVO_MODE_MID:
			fsm_set_state(SERVO_MODE2);
			break;
		default:
			fsm_set_state(fsm_get_state());
			break;
		}
		fsm_unlock();
	}
}



