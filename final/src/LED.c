#include "scoreboard.h"

void led_init() {
	RCC->AHB2ENR |= (RCC_AHB2ENR_GPIOCEN);
	SET_REG(GPIOC->MODER, (GPIO_MODER_MODE5 | GPIO_MODER_MODE6 | GPIO_MODER_MODE8 | GPIO_MODER_MODE9), 0x145<<10); // output
}

void led_quarter() {
	quarter = (quarter+1) % 4;
	if(quarter == 0) {
		//GPIOC->BSRR |= 5
	}
}
