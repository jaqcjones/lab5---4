/*
 * systick.h - routines and definitions for the systick handler
 */
#pragma once

#include "stdint.h"

volatile typedef struct {
	uint32_t CS;		/* Control and Status Register*/
	uint32_t RV;		/* Reload Value Register */
	uint32_t CV;		/* Current Value Register */
} SYSTICK_TypeDef;

#define SYSTICK_BASE (0xE000E010)				/* Starting at CS register */
#define SYSTICK ((SYSTICK_TypeDef*)SYSTICK_BASE)/* A pointer to first register, CS*/

#define WAIT_CYCLES 16000000

void __attribute__ ((interrupt)) systick_handler(void);
void systick_init(uint32_t end_val);
void systick_on(void);
void systick_off(void);
void systick_1sec_callback_fn(void);

