#include "stm32l476xx.h"

#define SYST_CSR	(*((uint32_t *)0xE000E010))   // control and status register
#define SYST_RVR	(*((uint32_t *)0xE000E014))   // reload value register
#define SYST_CVR	(*((uint32_t *)0xE000E018))   // current value register
#define SYST_CALIB	(*((uint32_t *)0xE000E01C))

TIM_TypeDef	*timer = TIM3;
int value, counter;
int table[4][4] = {{1, 2, 3, 10},
				   {4, 5, 6, 11},
				   {7, 8, 9, 12},
				   {15, 0, 14, 13}};

void GPIO_init_AF() {

	// choose PB4
	RCC->AHB2ENR = RCC->AHB2ENR | 0x2;
	GPIOB->MODER &= ~GPIO_MODER_MODE4;
	GPIOB->MODER |= GPIO_MODER_MODE4_1;  // set to the alternative function mode
	GPIOB->AFR[0] |= GPIO_AFRL_AFSEL4_1; // (TIM3_CH1)
}

void Keypad_init() {
	RCC->AHB2ENR = RCC->AHB2ENR | 0x7;

	// Set keypad GPIO OUTPUT
	// PA8: Col 1, PA9: Col 2, PA10: Col 3, PA12: Col 4
	// Set PA8,9,10,12 as output mode
	GPIOA->MODER = GPIOA->MODER & 0xFCC0FFFF;
	GPIOA->MODER = GPIOA->MODER | 0x01150000;

	// Set PA8,9,10,12 is Pull-up output
	GPIOA->PUPDR = GPIOA->PUPDR & 0xFCC0FFFF;
	GPIOA->PUPDR = GPIOA->PUPDR | 0x01150000;

	// Set PA8,9,10,12 as medium speed mode
	GPIOA->OSPEEDR = GPIOA->OSPEEDR & 0xFCC0FFFF;
	GPIOA->OSPEEDR = GPIOA->OSPEEDR | 0x01150000;

	// Set PA8,9,10,12 as high
	GPIOA->ODR = GPIOA->ODR | 0b10111 << 8;

	// SET keypad gpio INPUT
	// PB5: Row 1, PB6: Row 2, PB7: Row 3, PB9: Row 4
	// Set PB5,6,7,9 as INPUT mode
	GPIOB->MODER = GPIOB->MODER & 0xFFF303FF;

	// set PB5,6,7,9 is Pull-down input
	GPIOB->PUPDR = GPIOB->PUPDR & 0xFFF303FF;
	GPIOB->PUPDR = GPIOB->PUPDR | 0x0008A800;

	// Set PB5,6,7,9 as medium speed mode
	GPIOB->OSPEEDR = GPIOB->OSPEEDR & 0xFFF303FF;
	GPIOB->OSPEEDR = GPIOB->OSPEEDR | 0x00045400;

	GPIOC->MODER = GPIOC->MODER & 0xF3FFFFFF;

}

int keypad_scan() {
	int flag_keypad = GPIOB->IDR & (0b10111 << 5);
	int flag_debounce;
	if (flag_keypad != 0) {
		int k = 45000;
		while (k != 0) {
			flag_debounce = GPIOB->IDR & (0b10111 << 5);
			k--;
		}
		if (flag_debounce != 0) {
			for (int i=0; i<4; i++) { //scan keypad from first column
				int position_c = i + 8;
				if (i == 3)	position_c++;

				// set PA8,9,10,12(column) low and set pin high from PA8
				GPIOA->ODR = (GPIOA->ODR & 0xFFFFE8FF) | (1 << position_c);

				for (int j=0; j<4; j++) { //read input from first row
					int position_r = j + 5;
					if(j == 3) position_r++;

					// set PB5, 6, 7, 9(row)
					int flag_keypad_r = GPIOB->IDR & (1 << position_r);
					if (flag_keypad_r != 0)	return table[j][i];
				}
			}
		}
	}

	return 0;
}

void Timer_init(TIM_TypeDef *timer) {
	// Use TIM3
	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM3EN;
	timer->ARR = (uint32_t)99;
	timer->PSC = (uint32_t)152;
	timer->EGR = TIM_EGR_UG;
}

void PWM_channel_init(TIM_TypeDef *timer) {
	// Use CH1

	// due to ARR == 100
	timer->CCR1 = 50;

	// set bit[6:4] = 011, wave flip if CNT == CCR1
	// timer->CCMR1 &= 0xFF8F;
	timer->CCMR1 |= TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1;

	timer->CR1 |= TIM_CR1_ARPE;

	// Capture Compare 1 Output Enable
	timer->CCER |= TIM_CCER_CC1E;

	timer->EGR = TIM_EGR_UG;
}

void SystemClock_Config() {
	SYST_CSR &= (uint32_t) 0xFFFFFFF8;
	SYST_CSR |= 0x6;
	SYST_RVR = 3999999;
}

void EXTI_config() {
	// clock enable
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

	// manage the external interrupt to GPIOs
	SYSCFG->EXTICR[3] |= SYSCFG_EXTICR4_EXTI13_PC;

	// set up the interrupt mask register EXTI_IMR1
	EXTI->IMR1 = 0;
	EXTI->IMR1 |= EXTI_IMR1_IM13;

	// set the rising-triggered interrupt
	EXTI->RTSR1 |= EXTI_RTSR1_RT13;
}

void NVIC_config() {
	NVIC_EnableIRQ(EXTI15_10_IRQn);
	NVIC_SetPriority(EXTI15_10_IRQn, 1);
	NVIC_SetPriority(SysTick_IRQn, 0);
}

void SysTick_Handler(void) {
	// counter plus 1 every second
	counter++;
	if (counter == value) {
		timer->CR1 |= TIM_CR1_CEN;
		GPIOB->ODR ^= ( 0x0 | (1 << 3) );
		SYST_CSR &= (uint32_t) 0xFFFFFFF8;
	}
}

void EXTI15_10_IRQHandler(void) {
	timer->CR1 &= ~TIM_CR1_CEN;
	counter = value = 0;

	// clear pending, or interrupt will be triggered continually
	EXTI->PR1 = 1 << 13;

	return;
}

int main() {

	GPIO_init_AF();
	Keypad_init();
	Timer_init(timer);
	PWM_channel_init(timer);
	SystemClock_Config();
	EXTI_config();
	NVIC_config();

	value = 0;
	counter = 0;
	while (1) {
		GPIOA->ODR = GPIOA->ODR | (0b10111 << 8);
		if (!value) {
			value = keypad_scan();
			if (value > 0) {
				SYST_CSR |= 0x7;
			}
		}

	}

	return 0;
}
