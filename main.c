#include "fsm.h"
#include "USART2.h"
#include "USART3.h"
#include "userbutton.h"
#include "servo.h"
#include "systick.h"
#include "LED.h"

static int check_cmd = 0;
static int check_exit = 0;
//extern uint32_t cmd;
//extern uint32_t ping;
//extern uint32_t adc;
extern uint32_t servo;
extern uint32_t prev_case;
extern uint32_t send_cmd_message;
extern uint32_t send_exit_message;
extern uint32_t send_ping_message;
extern uint32_t send_update_message;
//extern uint32_t servo_mode;
extern int ping_index;
extern uint8_t ping_array[30];
extern int adc_index;
extern uint8_t adc_array[500];
extern int servo_index;
extern uint8_t servo_array[20];

void USART2_callback_fn(uint8_t byte) {
	USART3_putchar(byte);		/* forward to wifi module */
//	if(servo_mode == 1) {
//		USART2_putchar(byte);
//		/* If Enter pressed, stopped collecting */
//		if(byte == 13) servo_mode = 0;
//		else {
//			servo_array[servo_index] = byte;
//			servo_index++;
//		}
//		if(servo_index == 8) servo_mode = 0;
//	}
	if(prev_case == servo) {
		USART2_putchar(byte);
		if(fsm_lock() == FSM_LOCK_ACQUIRED) {
			switch(fsm_get_state()) {
			case SERVO_MODE1:
	//			if(servo_index <= 8)
				if(byte == '\r') {
					fsm_set_state(SERVO_MODE_MID);
				}
				else if(servo_index >= 19) {
					fsm_set_state(SERVO_MODE_MID);
				}
				else {
					servo_array[servo_index] = byte;
					servo_index++;
				}
				break;
			case SERVO_MODE_MID:
			case SERVO_MODE2:
				fsm_set_state(SERVO_MODE1);
				break;
			}
			fsm_unlock();
		}
	}
}

void USART3_callback_fn(uint8_t byte) {
	/* Buffer receiving ping message */
	if(send_ping_message == 1) {
		ping_array[ping_index] = byte;
		ping_index++;
	}
	/* Buffer receiving update message */
	else if(send_update_message == 1) {
		adc_array[adc_index] = byte;
		adc_index++;
	}

	/* Fail safe */
	/* If wifi already in CMD mode, stop trying to access CMD mode*/
	if(send_cmd_message == 1) {
		if(byte == '$')
			send_cmd_message = 0;
	}

	/* Check that CMD mode entered on wifly */
	if(byte == 'C') {
		check_cmd = 1;
	}
	else if(byte == 'M' && check_cmd == 1) {
		check_cmd = 2;
	}
	else if(byte == 'D' && check_cmd == 2) {
		check_cmd = 0;
		send_cmd_message = 0;
	}

	/* Check that we have EXITed CMD mode on wifly */
	if(byte == 'E') {
		check_exit = 1;
	}
	else if(byte == 'X' && check_exit == 1) {
		check_exit = 2;
	}
	else if(byte == 'I' && check_exit == 2) {
		check_exit = 3;
	}
	else if(byte == 'T' && check_exit == 3) {
		check_exit = 0;
		send_exit_message = 0;
	}

	USART2_putchar(byte);		/* forward to console */
}

void userbutton_callback_fn(void) {

	if(fsm_lock() == FSM_LOCK_ACQUIRED) {
		fsm_set_state(UB_DEBOUNCE);
	}
	fsm_unlock();
}

void main(void)
{
	/* Set up the USART2 9600-8N1 and to call USART2_callback_fn when new data arrives */
	USART2_init(USART2_callback_fn);

	USART3_init(USART3_callback_fn);

	LED_init();
	ADC_init();

	/* Configure user pushbutton and call pushbutton_callback_fn when button press-released */
 	userbutton_init(userbutton_callback_fn);

 	/* Initialize the servo */
 	TIM4_CH1_init();

	/* Enable interrupts - do this after initializing the system */
	__asm ("  cpsie i \n" );

	/* initialize the finite state machine */
	fsm_init();

	/* Wait here forever - everything is now interrupt driven */
	while(1)
	{
		;;;
	}
}

