#include <stdint.h>

typedef enum {
	CMD_MODE1 = 0,
	CMD_MODE2,
	UB_DEBOUNCE,
	UB_DEBOUNCE2,
	PING_MODE1,
	PING_MODE2,
	POT_UPDATE1,
	POT_UPDATE2,
	SERVO_MODE1,
	SERVO_MODE_MID,
	SERVO_MODE2
} state_t;

#define PING 1
#define MYID 17

typedef struct {
	int type;
	int id;
} Ping_t;

#define UPDATE 2

typedef struct {
	int type;
	int id;
	int value;
} Update_req_t;

typedef struct {
	int type;
	int id;
	int average;
	int values[30];
} Update_resp_t;

/* Initialize the finite state machine */
void fsm_init(void);

/* Acquire a lock on the fsm before accessing/altering state */
#define FSM_LOCK_FAILED   (0)
#define FSM_LOCK_ACQUIRED (1)
uint8_t fsm_lock(void);
void fsm_unlock(void);

/* Query FSM state - only do this when you have acquired the lock! */
state_t fsm_get_state(void);

/* Alter FSM state - only do this when you have acquired the lock! */
void fsm_set_state(state_t new_state);

void enter_cmd_mode(void);
void exit_cmd_mode(void);
void transmit_ping(void);
void display_ping_rx(void);
void bits_to_hex(uint32_t val);
void send_adc_update(void);
void display_adc_rx(void);
void move_servo(void);
