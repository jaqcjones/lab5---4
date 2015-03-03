#include <fsm.h>

#include <LED.h>
#include <mutex.h>
#include <USART2.h>
#include <servo.h>
#include <systick.h>
#include "ADC.h"
#include "servo.h"

static uint32_t fsm_mutex;

static state_t state = CMD_MODE1;

uint32_t count1sec;
uint32_t count2sec;
uint32_t count5sec;
uint32_t cmd;
uint32_t ping;
uint32_t adc;
uint32_t servo;
uint32_t prev_case;
uint32_t send_cmd_message;
uint32_t send_exit_message;
uint32_t send_ping_message;
uint32_t send_update_message;
uint32_t servo_mode;

Ping_t ping_message;
Ping_t *p;
int ping_index;
uint8_t ping_array[30] = {'\0'};

Update_req_t update_message;
Update_req_t *u;
int adc_index;
uint8_t adc_array[500] = {'\0'};

Update_resp_t *update_response;
//Update_resp_t *r;

int servo_index;
uint8_t servo_array[10] = {'\0'};

void enter_cmd_mode(void);
void exit_cmd_mode(void);
void transmit_ping(void);
void display_ping_rx(void);
void bits_to_hex(uint32_t val);
void send_adc_update(void);
void display_adc_rx(void);
void move_servo(void);

void fsm_init(void)
{
	count1sec = 1;
	count2sec = 2;
	count5sec = 5;
	cmd = 1;
	ping = 5;
	adc = 10;
	servo = 15;
	send_cmd_message = 0;
	send_exit_message = 0;
	send_ping_message = 0;
	send_update_message = 0;
	servo_mode = 0;
	ping_index = 0;
	adc_index = 0;
	servo_index = 0;

//	Ping_t ping_message;
	ping_message.type = PING;
	ping_message.id = MYID;
	p = &ping_message;

	update_message.type = UPDATE;
	update_message.id = MYID;
//	update_message.value
	u = &update_message;

	mutex_init(&fsm_mutex);
	while( fsm_lock() != FSM_LOCK_ACQUIRED );
	fsm_set_state(CMD_MODE1);
	fsm_unlock();
}

uint8_t fsm_lock(void)
{
	uint8_t retval = FSM_LOCK_FAILED;

	if( mutex_lock(&fsm_mutex) == MUTEX_LOCKED )
		retval = FSM_LOCK_ACQUIRED;

	return retval;
}

void fsm_unlock(void)
{
	mutex_unlock(&fsm_mutex);
}

state_t fsm_get_state(void)
{
	return state;
}

void fsm_set_state(state_t new_state) {
	if(fsm_mutex == MUTEX_LOCKED) {
		state = new_state;

		switch(state) {

		case CMD_MODE1:
		default:
			USART2_putstr("STATE: CMD_MODE1\r\n");
			/* Set prev_case to cmd */
			prev_case = cmd;
			/* Enter command mode */
			send_cmd_message = 1;
			enter_cmd_mode();
			/* Set flag to send exit cmd when UB pressed*/
			send_exit_message = 1;
			/* Wait 1sec for cmd to go through */
			systick_init(count1sec);
			break;
		case CMD_MODE2:
			/* Blue LED on, GRO off */
			LED_update(LED_BLUE_ON | LED_RED_OFF | LED_ORANGE_OFF | LED_GREEN_OFF);
			USART2_putstr("STATE: CMD_MODE2\r\n");
			break;
		case UB_DEBOUNCE2:
		case UB_DEBOUNCE:
			/* Lights off */
			LED_update(LED_BLUE_OFF | LED_RED_OFF | LED_ORANGE_OFF | LED_GREEN_OFF);
			/* Exit command mode only if coming from CMD_MODE*/
			exit_cmd_mode();
			/* Wait 1sec to debounce */
			systick_init(count1sec);
			break;
		case PING_MODE1:
			/* Orange on, GRB off */
			LED_update(LED_BLUE_OFF | LED_RED_OFF | LED_ORANGE_ON | LED_GREEN_OFF);
			/* Set prev_case to ping */
			prev_case = ping;
			USART2_putstr("STATE: PING_MODE1\r\n");
			/* Send ping message */
			send_ping_message = 1;
			ping_index = 0;
			transmit_ping();
			/* Wait 1sec */
			systick_init(count1sec);
			break;
		case PING_MODE2:
			/* Orange on, GRB off */
			LED_update(LED_BLUE_OFF | LED_RED_OFF | LED_ORANGE_ON | LED_GREEN_OFF);
			USART2_putstr("STATE: PING_MODE2\r\n");
			/* Stop sending and receiving ping message */
			send_ping_message = 0;
			/* Display received ping message on console */
			display_ping_rx();
			/* Go directly back to PING_MODE1 */
			fsm_set_state(PING_MODE1);
			break;
		case POT_UPDATE1:
			send_ping_message = 0;
			/* Green on, ORB off */
			LED_update(LED_BLUE_OFF | LED_RED_OFF | LED_ORANGE_OFF | LED_GREEN_ON);
			/* Set prev_case to adc */
			prev_case = adc;
			USART2_putstr("STATE: POT_UPDATE1\r\n");
			/* Send update message */
			send_update_message = 1;
			adc_index = 0;
			send_adc_update();
			/* Wait 1sec*/
			systick_init(count2sec);
			break;
		case POT_UPDATE2:
			/* Green on, ORB off */
			LED_update(LED_BLUE_OFF | LED_RED_OFF | LED_ORANGE_OFF | LED_GREEN_ON);
			USART2_putstr("STATE: POT_UPDATE2\r\n");
			/* Stop sending and receiving update message */
			send_update_message = 0;
			/* Display received ping message on console */
			display_adc_rx();
			/* Go directly back to POT_UPDATE1 */
			fsm_set_state(POT_UPDATE1);
			break;
		case SERVO_MODE1:
			/* Red on, GOB off */
			LED_update(LED_BLUE_OFF | LED_RED_ON | LED_ORANGE_OFF | LED_GREEN_OFF);
			/* Set prev_case to servo */
			prev_case = servo;
			USART2_putstr("Enter ID: ");
			servo_index = 0;
//			servo_mode = 1;
//			systick_init(count5sec);
			break;
		case SERVO_MODE_MID:
			/* Red on, GOB off */
			LED_update(LED_BLUE_OFF | LED_RED_ON | LED_ORANGE_OFF | LED_GREEN_OFF);
			USART2_putstr("\r\nSERVO_MODE_MID\r\n");
			/* Send update message */
			send_update_message = 1;
			adc_index = 0;
			send_adc_update();
//			fsm_set_state(SERVO_MODE2);
			systick_init(count2sec);
			break;
		case SERVO_MODE2:
			/* Red on, GOB off */
			LED_update(LED_BLUE_OFF | LED_RED_ON | LED_ORANGE_OFF | LED_GREEN_OFF);
			USART2_putstr("\r\nSERVO_MODE2\r\n");
			/* Stop sending and receiving update message */
			send_update_message = 0;
//			if(servo_index == 0) {
//				fsm_set_state(SERVO_MODE1); /* If no ID given, request again */
//			}
//			else {
				move_servo();
				fsm_set_state(SERVO_MODE_MID);
//			}
			break;
		}
	}
}

void enter_cmd_mode(void) {
	if(send_cmd_message == 1) {
		USART3_putchar(36);
		USART3_putchar(36);
		USART3_putchar(36);
	}
}

void exit_cmd_mode(void) {
	if(send_exit_message == 1) {
		USART3_putstr("exit\r\n");
	}
}

void transmit_ping(void) {
	int s = sizeof(Ping_t);
	int i = 0;
	uint8_t *ptr = (uint8_t*)p;
	if(send_ping_message == 1) {
		for(i=0;i<s;i++){
			USART3_putchar(*ptr);
			ptr++;
		}
	}
}

void display_ping_rx(void) {
	int s = sizeof(Ping_t)/4;
	int i = 0;
	uint8_t *ptr = (uint8_t*)p;
	uint32_t *msg_int = (uint32_t*)ptr;
	USART2_putstr("Ping Message:\r\n ");
	for(i=0;i<s;i++){
		bits_to_hex(*msg_int);
		msg_int++;
		USART2_putstr("\r\n");
	}
	USART2_putstr("Message Received:\r\n ");

	uint32_t *recv_int;
	recv_int = (uint32_t*)ping_array;
	int h = 0;
	int end_index = ping_index/4; /* Changing 8 bit array to 32 bit array*/
	for(h=0;h<end_index;h++) {
		bits_to_hex(*recv_int);
		recv_int++;
		USART2_putstr("\r\n");
	}
}

void bits_to_hex(uint32_t val){
	char ans[10];
	int mod_val = 0;
	int i;
	USART2_putchar('0');
	USART2_putchar('x');
	for (i=0;val!=0; i++){
		mod_val = val % 16;
		if (mod_val < 10) {
			mod_val = mod_val + 48;
		}
		else {
			mod_val = mod_val + 55;
		}
		ans[i] = mod_val;
		val = val / 16;
	}
	while (i != 0) {
		USART2_putchar(ans[i-1]);
		i--;
	}
	return;
}

void send_adc_update(void) {
	int dummy_index = 0;
	long dummy_array[1] = {0};
	int pot_val = read_ADC(dummy_array, &dummy_index, 0);
	update_message.value = pot_val;	/* Get position of potentiometer */
	int s = sizeof(Update_req_t);
	int i = 0;
	uint8_t *ptr = (uint8_t*)u;
	if(send_update_message == 1) {
		for(i=0;i<s;i++){
			USART3_putchar(*ptr);
			ptr++;
		}
	}
}

void display_adc_rx(void) {
	int s = sizeof(Update_req_t)/4;
	int i = 0;
	uint8_t *ptr = (uint8_t*)u;
	uint32_t *msg_int = (uint32_t*)ptr;
	USART2_putstr("Update Message:\r\n ");
	for(i=0;i<s;i++){
		bits_to_hex(*msg_int);
		msg_int++;
		USART2_putstr("\r\n");
	}

	update_response = (Update_resp_t*)adc_array;
	USART2_putstr("Update Message Received:\r\n ");
	USART2_putstr("Avg: ");
	bits_to_hex(update_response->average);
	USART2_putstr("\r\n");

	/* Position servo based on resp */
	servo_test(update_response->values[17]);
}

void move_servo(void) {
	int newID = (servo_array[0]-48)*10 + (servo_array[1]-48);
	update_response = (Update_resp_t*)adc_array;
	/* Position servo based on resp */
	servo_test(update_response->values[newID]);
}

