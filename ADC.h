#pragma once

#include "stdint.h"

volatile typedef struct {
	uint32_t SR;
	uint32_t CR1;
	uint32_t CR2;
	uint32_t SMPR1;
	uint32_t SMPR2;
	uint32_t JOFR1;
	uint32_t JOFR2;
	uint32_t JOFR3;
	uint32_t JOFR4;
	uint32_t HTR;
	uint32_t LTR;
	uint32_t SQR1;
	uint32_t SQR2;
	uint32_t SQR3;
	uint32_t JSQR;
	uint32_t JDR1;
	uint32_t JDR2;
	uint32_t JDR3;
	uint32_t JDR4;
	uint32_t DR;

} ADC_TypeDef ;

#define GPIOx_MODER_PIN1_ANALOGIN	(0x0000000C)
#define RCC_APB2ENR_ADC1EN			(0x0100)

#define ADC1_BASE				(0x40012000)
#define ADC1					((ADC_TypeDef*)ADC1_BASE)

#define ADC1_SQR1_LEN1			(0x00000000)		/* Regular sequence length 1 */
#define ADC1_SQR3_CONV1_ADCIN1	(0x01)				/*channel ADC_IN1 (0x01)*/

#define ADC1_SMPR2_SMP1			(0x08)		/* 480 cycles for potentiometer on SMP1 */
#define ADC1_CR2_SET_ADON		(0x01)
#define ADC1_CR2_SET_SWSTART	(0x40000000)
#define ADC1_SR_EOC_MASK		(0x02)

#define GPIOA_USERB_MASK		(0x01)

void ADC_init(void);
long read_ADC(long *array, int *index, int update_array);
