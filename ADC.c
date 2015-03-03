/*
 * ADC.c
 * Deals with potentiometer stuff
 * Deals with user button stuff
 */

#include "ADC.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "USART2.h"
//#include "mutex.h"

void ADC_init(void) {
	/* Enable GPIOA port peripheral clock */
	RCC->AHB1ENR = RCC->AHB1ENR | RCC_AHB1ENR_GPIOAEN;

	/* Set Pin 1 as analog input (THIS WAS THE PROBLEM - DO OR's ALWAYS!!) */
	GPIOA->MODER = GPIOA->MODER | GPIOx_MODER_PIN1_ANALOGIN;

	/* Enable ADC peripheral clock */
	RCC->APB2ENR = RCC->APB2ENR | RCC_APB2ENR_ADC1EN;

	/* Configure ADC1_SQR1 for sequence length of 1 */
	ADC1->SQR1 = ADC1->SQR1 & ADC1_SQR1_LEN1;

	/* Map 1st reg conversion to ADC channel for potentiometer*/
	ADC1->SQR3 = ADC1->SQR3 | ADC1_SQR3_CONV1_ADCIN1;

	/* Set sample time on SMP1 (ADC_IN1) to 480 cycles */
	ADC1->SMPR2 = ADC1->SMPR2 | ADC1_SMPR2_SMP1;

	/* Enable conversions */
	ADC1->CR2 = ADC1->CR2 | ADC1_CR2_SET_ADON;

}
/*
 * Reads values from potentiometer
 * Returns an array of last 4 values
 * returns the last value of the potentiometer
 */
long read_ADC(long *array, int *index, int update_array) {
	/* Initiate conversion */
	ADC1->CR2 = ADC1->CR2 | ADC1_CR2_SET_SWSTART;

	/* Read converted value */
	/* Check for end of conversion */
	while ((ADC1->SR & ADC1_SR_EOC_MASK) != ADC1_SR_EOC_MASK) {
		/* Wait */
	}
	/* Read conversion value */
	long conv_val = ADC1->DR;
	/* Convert value to hex and print to terminal */
	//bits_to_hex(conv_val);

	/*Only update array if user button pressed*/
	if (update_array == 1) {
		array[*index] = conv_val;
		*index = *index + 1;
		*index = *index % 4;
	}

	return conv_val;

}

