#include "scoreboard.h"

void buzzer_init() { // pc11
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;
	SET_REG(GPIOC->MODER, GPIO_MODER_MODE11, 1<<22); // output
//	GPIOC->BSRR |= 1<<11;
}

void buzzer_update() {
	if(TIM4->CNT == 0) {
		buzz = 0;
	}
	if(buzz) {
		GPIOC->BSRR |= 1<<11;
	}
	else GPIOC->BRR |= 1<<11;
}

void buzzer_long() {
	buzz = 1;
	TIM4->CNT = 1499;
}

void buzzer_short() {
	buzz = 1;
	TIM4->CNT = 499;
}

