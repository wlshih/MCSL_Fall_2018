#include "scoreboard.h"

void score_init() { // pc0, 1, 2, 3, pb0
	RCC->AHB2ENR |= (RCC_AHB2ENR_GPIOBEN | RCC_AHB2ENR_GPIOCEN);
	SET_REG(GPIOB->MODER, GPIO_MODER_MODE0, 1);
	SET_REG(GPIOC->MODER, (GPIO_MODER_MODE0 | GPIO_MODER_MODE1 | GPIO_MODER_MODE2 | GPIO_MODER_MODE3), 0x55); // output
}

void score_host_add() { // A
	GPIOB->BSRR |= 1;

	GPIOC->BSRR|= 1;
	GPIOC->BRR|= 1<<1;
	GPIOC->BSRR|= 1<<2;
	GPIOC->BRR|= 1<<3;

//	GPIOB->BRR |= 1;
}

void score_host_sub() { // B
	GPIOB->BSRR |= 1;

	GPIOC->BSRR|= 1;
	GPIOC->BRR|= 1<<1;
	GPIOC->BSRR|= 1<<2;
	GPIOC->BSRR|= 1<<3;

//	GPIOB->BRR |= 1;
}

void score_guest_add() { // C
	GPIOB->BSRR |= 1;

	GPIOC->BSRR|= 1;
	GPIOC->BSRR|= 1<<1;
	GPIOC->BRR|= 1<<2;
	GPIOC->BRR|= 1<<3;

//	GPIOB->BRR |= 1;
}

void score_guest_sub() { // D
	GPIOB->BSRR |= 1;

	GPIOC->BSRR|= 1;
	GPIOC->BSRR|= 1<<1;
	GPIOC->BRR|= 1<<2;
	GPIOC->BSRR|= 1<<3;

//	GPIOB->BRR |= 1;
}

void next_quarter() { // #
	GPIOB->BSRR |= 1;

	GPIOC->BSRR|= 1;
	GPIOC->BSRR|= 1<<1;
	GPIOC->BSRR|= 1<<2;
	GPIOC->BRR|= 1<<3;

//	GPIOB->BRR |= 1;
}
