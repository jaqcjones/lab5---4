
#include "servo.h"
#include <stm32f4xx_rcc.h>
#include <stm32f4xx_gpio.h>

//uint32_t GATE_OPEN = 2100;
//uint32_t GATE_CLOSED = 1100;

void TIM4_CH1_init(void) {
	/* Enable GPIOB peripheral clock */
	RCC->AHB1ENR = RCC->AHB1ENR | RCC_AHB1ENR_GPIOBEN;

	/* Configure Port B pin 6*/
	GPIOB->MODER = GPIOB->MODER & GPIOB_MODER_PIN6_RESET_MASK;	/* Reset */
	GPIOB->MODER = GPIOB->MODER | GPIOB_MODER_PIN6_AF;	/* Set to AF */

	GPIOB->AFRL = GPIOB->AFRL & GPIOB_AFRL_PIN6_RESET_MASK;	/* Reset pin 6 AF settings */
	GPIOB->AFRL = GPIOB->AFRL | GPIOB_AFRL_PIN6_AF2;		/* Set to AF2 */

	/* Enable TIM4 peripheral clock */
	RCC->APB1ENR = RCC->APB1ENR | RCC_APB1ENR_TIM4EN;

	/* Configure CCMR1 OC1M field as PWM mode 1 */
	TIM4->CCMR1 = TIM4->CCMR1 & TIM4_CCMR1_OC1M_RESET_MASK;
	TIM4->CCMR1 = TIM4->CCMR1 | TIM4_CCMR1_OC1M_PWM1;

	/* Configure CCER CC1E field as 'enabled' */
	TIM4->CCER = TIM4->CCER | TIM4_CCER_CC1E;

	/* Fundamental frequency = 50 Hz */
	TIM4->PSC = TIM4->PSC & TIM4_PSC_RESET_MASK;
	TIM4->PSC = TIM4->PSC | TIM4_PSC;
	TIM4->ARR = TIM4->ARR & TIM4_ARR_RESET_MASK;
	TIM4->ARR = TIM4->ARR | TIM4_ARR;

	/* Duty cycle = 5.5% (1.1 ms for 20 ms period); Set 55 in CCR1*/
	TIM4->CCR1 = TIM4->CCR1 & TIM4_CCR1_RESET_MASK;
	TIM4->CCR1 = TIM4->CCR1 | TIM4_CCR1;

	/* Enable TIM4 counter */
	TIM4->CR1 = TIM4->CR1 | TIM4_CR1_CEN;
}

void servo_test(uint32_t degree) {
	/* Vary duty cycle */
//	int i;
	degree = (degree*220)/819 + 1000;
	TIM4->CCR1 = degree;

}

