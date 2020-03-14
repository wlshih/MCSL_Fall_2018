#include "stm32l476xx.h"
#include "button.h"

/**********************************************************************
 * connected to		pin name	MCU pin		mode
 * ===============================================================
 *
 * User button		--		PC13		input
 *
 **********************************************************************/

void btn_init (void)
{
	/* enable AHB2 clock for port C */
	RCC->AHB2ENR |= 0x1 << 2;

	/* set PC13 as input mode */
	GPIOC->MODER &= ~ GPIO_MODER_MODE13;

	/* set PC13 as medium speed mode */
	GPIOC->OSPEEDR &= ~ GPIO_OSPEEDR_OSPEED13;
	GPIOC->OSPEEDR |= GPIO_OSPEEDR_OSPEED13_0;
}

int btn_pressed (void)
{
	int	deb = 0;
	while (! (GPIOC->IDR & (GPIO_IDR_ID13)) && deb++ < 1000);
	return ! (GPIOC->IDR & (GPIO_IDR_ID13));
}
