#include "stm32l476xx.h"
#include "led.h"

/**********************************************************************
 * connected to		pin name	MCU pin		mode
 * ===============================================================
 *
 * on-board LED		D13		PA5		output
 *
 **********************************************************************/

void led_init (void)
{
	/* enable AHB2 clock for port A */
	RCC->AHB2ENR |= 0x1;

	/* set PA5 as output mode */
	GPIOA->MODER &= ~ GPIO_MODER_MODE5;
	GPIOA->MODER |= GPIO_MODER_MODE5_0;

	/* set PA5 as medium speed mode */
	GPIOA->OSPEEDR &= ~ GPIO_OSPEEDR_OSPEED5;
	GPIOA->OSPEEDR |= GPIO_OSPEEDR_OSPEED5_0;
}

void led_on (void)
{
	GPIOA->BSRR = GPIO_BSRR_BS5;
}

void led_off (void)
{
	GPIOA->BRR = GPIO_BRR_BR5;
}

void led_toggle (void)
{
	if (GPIOA->ODR & GPIO_ODR_OD5)
		led_off ();
	else
		led_on ();
}
