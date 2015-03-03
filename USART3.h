#pragma once

#include "stdint.h"

/*
 * Initialize the USART to 9600 baud, 8 data bits, no parity, one stop bit.
 * Received data are passed back using the USART2_rx_callback() function passed in to init.
 */
void USART3_init(void(*USART2_rx_callback)(uint8_t byte));

/* Send a single character out USART2 */
void USART3_putchar(uint8_t byte);

/* Send a null-terminated string out USART2 */
void USART3_putstr(uint8_t *buffer);
