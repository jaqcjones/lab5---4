#include <stm32f4xx_gpio.h>

/*
 * Set up TIM3_CH1 on PB4
 */
void timer_init(void) {
	/* Enable GPIOB peripheral clock */
//	RCC->AHB1ENR = RCC->AHB1ENR | RCC_AHB1ENR_GPIOBEN;

	/* Configure Port B pin 4 */
//	GPIOB

	/* Set OPM (One-Pulse Mode) bit in TIMx_CR1 */

	/* Set Update Interrupt Enable bit in TIMx_DIER */

	/* Set CEN (Counter enable) bit in TIMx_CR1 */

}
