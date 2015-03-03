
#include <stm32f4xx_usart.h>
#include <stm32f4xx_gpio.h>
#include <stm32f4xx_rcc.h>
#include <cortexm4_nvic.h>
#include <USART3.h>

/* flag to indicate USART2 has been initialized */
/* toggled in init function */
static uint32_t initialized = 0;

/* module-global pointer to the callback function for rx'd bytes */
/* populated in init function. */
static void(*rx_callback_fn)(uint8_t byte);


/* Rudimentary handler assumes the interrupt is due to a byte rx event */
void __attribute__ ((interrupt)) USART3_handler(void)
{
	uint8_t byte;

	/* must read the USART3_DR to clear the interrupt */
	byte = USART3->DR;

	if( rx_callback_fn )
	{
		rx_callback_fn(byte);
	}
}

void USART3_init(void(*USART3_rx_callback)(uint8_t byte))
{
	rx_callback_fn = USART3_rx_callback;

	/* Enable GPIOD, as USART3 TX is on PD8, and RX is on PD9 */
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;

	/* Configure PD8 as AF7 (Output, push-pull) */
	GPIOD->AFRH &= GPIOx_AFRH_AFRH8_MASK;
	GPIOD->AFRH |= GPIOx_AFRH_AFRH8_AF7;
	GPIOD->MODER &= GPIOx_MODER_PIN8_MASK;
	GPIOD->MODER |= GPIOx_MODER_PIN8_AF;

	/* Configure PD9 as AF7 (Input) */
	GPIOD->AFRH &= GPIOx_AFRH_AFRH9_MASK;
	GPIOD->AFRH |= GPIOx_AFRH_AFRH9_AF7;
	GPIOD->MODER &= GPIOx_MODER_PIN9_MASK;
	GPIOD->MODER |= GPIOx_MODER_PIN9_AF;

	/* Reset the USART peripheral and enable its clock */
	RCC->APB1ENR &= ~RCC_APB1ENR_USART3;	/**/
	RCC->APB1RSTR |= RCC_APB1RSTR_USART3;
	RCC->APB1RSTR &= ~RCC_APB1RSTR_USART3;
	RCC->APB1ENR |= RCC_APB1ENR_USART3;

	/* Enable the USART peripheral */
	USART3->CR1 |= USARTx_CR1_UE;  /* Enable */

	/* Configure for 8N1, 9600 baud (assuming 16MHz clock) */
	USART3->BRR = 1667;             /* 16MHz/1667 ~= 9600 */

	/* Enable transmit */
	USART3->CR1 |= (USARTx_CR1_TE);

	/*
	 *  If a callback function was registered, enable receive and the
	 *  receive interrupt.
	 */
	if( USART3_rx_callback )
	{
		/* Configure Receive Interrupt */
		NVIC_INTERRUPT_USART_2_ENABLE();
		USART3->CR1 |= USARTx_CR1_RXNEIE;
		USART3->CR1 |= (USARTx_CR1_RE);
	}

	initialized = 1;
}

/* Send a single character out USART3 */
void USART3_putchar(uint8_t byte)
{
	if( initialized )
	{
		/* Wait for the transmit shift register to be free... */
		while( !(USART3->SR & USARTx_SR_TXE) );
		USART3->DR = byte;
	}
}

/* Send a null-terminated string out USART3 */
void USART3_putstr(uint8_t *buffer)
{
	if( initialized )
	{
		while( *buffer != '\0')
		{
			USART3_putchar(*buffer);
			buffer++;
		}
	}
}
