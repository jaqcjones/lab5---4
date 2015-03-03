#pragma once

#include "stdint.h"

volatile typedef struct {
	uint32_t CR1;
	uint32_t CR2;
	uint32_t SMCR;
	uint32_t DIER;
	uint32_t SR;
	uint32_t EGR;
	uint32_t CCMR1;
	uint32_t CCMR2;
	uint32_t CCER;
	uint32_t CNT;
	uint32_t PSC;
	uint32_t ARR;
	uint32_t reserved1;
	uint32_t CCR1;
	uint32_t CCR2;
	uint32_t CCR3;
	uint32_t CCR4;
	uint32_t reserved2;
	uint32_t DCR;
	uint32_t DMAR;
} TIMx_TypeDef;

#define RCC_AHB1ENR_GPIOBEN		(0x02)
#define RCC_APB1ENR_TIM4EN		(0x04)

#define GPIOB_MODER_PIN6_RESET_MASK		(0xffffcfff)
#define GPIOB_MODER_PIN6_AF				(0x00002000)
#define GPIOB_AFRL_PIN6_RESET_MASK		(0xf0ffffff)
#define GPIOB_AFRL_PIN6_AF2				(0x02000000)

#define TIM4_BASE	(0x40000800)
#define TIM4		((TIMx_TypeDef*)TIM4_BASE)

#define TIM4_CCMR1_OC1M_RESET_MASK	(0xffffff8f)
#define TIM4_CCMR1_OC1M_PWM1		(0x00000060)
#define TIM4_CCER_CC1E				(0x00000001)

#define TIM4_PSC_RESET_MASK		(0x0)
#define TIM4_PSC				15//(319)		/* 320-1 = 319; gives a 50kHz clk */
#define TIM4_ARR_RESET_MASK		(0x0)
#define TIM4_ARR				19999//(0x3e8)		/* 1000 */
#define TIM4_CCR1_RESET_MASK	(0x0)
#define TIM4_CCR1				1500//(55)		/* Duty cycle = 5.5%; 1.1 ms; dec=55 */

#define TIM4_CR1_CEN	(0x01)

//extern uint32_t GATE_OPEN;
//extern uint32_t GATE_CLOSED;

void TM4_CH1_init(void);
void servo_test(uint32_t degree);


