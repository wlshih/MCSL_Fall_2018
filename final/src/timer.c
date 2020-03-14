#include "scoreboard.h"


void timer_init() { // TIM3 and TIM4
	RCC->APB1ENR1 |= (RCC_APB1ENR1_TIM3EN | RCC_APB1ENR1_TIM4EN);

	// game clock
	SET_REG(TIM3->CR1, TIM_CR1_CEN, TIM_CR1_CEN);
	SET_REG(TIM3->CR1, TIM_CR1_DIR, TIM_CR1_DIR); // downcount
	SET_REG(TIM3->EGR, TIM_EGR_UG, TIM_EGR_UG);
	TIM3->PSC = 3999;
	TIM3->ARR = 999; // 1ms

	// buzzer
	SET_REG(TIM4->CR1, TIM_CR1_CEN, TIM_CR1_CEN);
	SET_REG(TIM4->CR1, TIM_CR1_DIR, TIM_CR1_DIR); // downcount
	SET_REG(TIM4->EGR, TIM_EGR_UG, TIM_EGR_UG);
	TIM4->PSC = 3999; // 1 ms
	TIM4->ARR = 1999; // 2 sec
}
