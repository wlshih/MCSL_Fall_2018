#include "stm32l476xx.h"
#define SET_REG(REG,SELECT,VAL) {((REG)=((REG)&(~(SELECT))) | (VAL));};

#define SYST_CSR	(*((uint32_t *)0xE000E010))   // control and status register
#define SYST_RVR	(*((uint32_t *)0xE000E014))   // reload value register
#define SYST_CVR	(*((uint32_t *)0xE000E018))   // current value register
#define SYST_CALIB	(*((uint32_t *)0xE000E01C))

void GPIO_init();
void Timer_init();
void PWM_channel_init();
void SystemClock_config();
void System_config();
void EXTI_config();
void NVIC_config();

int keypad_scan();

void buzzer_on();
void buzzer_off();


int key_input = 0;
int buzz = 0;
int timer = 0;
int countdown = 0;

int main() {
	GPIO_init();
	Timer_init();
	PWM_channel_init();

	SystemClock_config();
	System_config();
	SysTick_Config(4000);
	EXTI_config();
	NVIC_config();
	buzzer_on();
	while(1) {
		key_input = keypad_scan();
		int deb = key_input;
		if(key_input) {
			timer = 1; // count from 1
			countdown = key_input;
			while(countdown);
			buzzer_on();
		}
	}
}

// enable LD2 on PA5
// keypad input on PB10, 15, 14, 11
// keypad output on PB2, 1, 5, 3
// user button on PC13
// buzzer on PB0
void GPIO_init() {
	RCC->AHB2ENR |= (RCC_AHB2ENR_GPIOAEN | RCC_AHB2ENR_GPIOBEN| RCC_AHB2ENR_GPIOCEN);

	// LED
	SET_REG(GPIOA->MODER, GPIO_MODER_MODE5, (uint32_t)1<<10);
	SET_REG(GPIOA->OSPEEDR, GPIO_OSPEEDR_OSPEED5, (uint32_t)2<<10);

	// keypad input
	SET_REG(GPIOB->MODER, (GPIO_MODER_MODE10 | GPIO_MODER_MODE15 | GPIO_MODER_MODE14 | GPIO_MODER_MODE11), 0);
	SET_REG(GPIOB->PUPDR, (GPIO_PUPDR_PUPD10 | GPIO_PUPDR_PUPD15 | GPIO_PUPDR_PUPD14 | GPIO_PUPDR_PUPD11), 0xA0A<<20);
	// keypad output
	SET_REG(GPIOB->MODER, (GPIO_MODER_MODE1 | GPIO_MODER_MODE2 | GPIO_MODER_MODE5 | GPIO_MODER_MODE3), (uint32_t)0x00000454u);
	SET_REG(GPIOB->OSPEEDR, (GPIO_OSPEEDR_OSPEED1 | GPIO_OSPEEDR_OSPEED2 | GPIO_OSPEEDR_OSPEED5 | GPIO_OSPEEDR_OSPEED3), (uint32_t)0x00000454u);

	// user button
	SET_REG(GPIOC->MODER, GPIO_MODER_MODE13, 0);
//	SET_REG(GPIOC->OTYPER, GPIO_OTYPER_OT13, 0);
	SET_REG(GPIOC->PUPDR, GPIO_PUPDR_PUPD13, 2<<26);

	// buzzer
	SET_REG(GPIOB->MODER, GPIO_MODER_MODE4, GPIO_MODER_MODE4_1); // AF
//	SET_REG(GPIOB->OSPEEDR, GPIO_OSPEEDR_OSPEED4, (uint32_t)0x10u<<8);
	GPIOB->AFR[0] |= GPIO_AFRL_AFSEL4_1; // af2 (TIM3_CH1)

//	GPIOA->BSRR |= GPIO_BSRR_BS5;
	GPIOB->BSRR |= (GPIO_BSRR_BS10 | GPIO_BSRR_BS15 | GPIO_BSRR_BS14 | GPIO_BSRR_BS11);
}

void Timer_init() {
	// Use TIM3
	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM3EN;
	TIM3->ARR = (uint32_t)99;
	TIM3->PSC = (uint32_t)381;
	TIM3->EGR = TIM_EGR_UG;
}

void PWM_channel_init() {
	// Use CH1

	// due to ARR == 100
	TIM3->CCR1 = 50;

	// set bit[6:4] = 011, wave flip if CNT == CCR1
	// timer->CCMR1 &= 0xFF8F;
	TIM3->CCMR1 |= TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1;

	TIM3->CR1 |= TIM_CR1_ARPE;
	// Capture Compare 1 Output Enable
	TIM3->CCER |= TIM_CCER_CC1E;
	TIM3->EGR = TIM_EGR_UG;

}

void SystemClock_config() {
	SYST_CSR &= (uint32_t) 0xFFFFFFF8;
	SYST_CSR |= 0x6;
	SYST_RVR = 3999999;

	SYST_CSR &= (uint32_t) 0xFFFFFFF8;
}

void System_config() {
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
	// keypad
	SYSCFG->EXTICR[2] |= SYSCFG_EXTICR3_EXTI10_PB;
	SYSCFG->EXTICR[3] |= SYSCFG_EXTICR4_EXTI15_PB;
	SYSCFG->EXTICR[3] |= SYSCFG_EXTICR4_EXTI14_PB;
	SYSCFG->EXTICR[2] |= SYSCFG_EXTICR3_EXTI11_PB;
	// user button
	SYSCFG->EXTICR[3] |= SYSCFG_EXTICR4_EXTI13_PC;
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

// interrupt handler implementation
void EXTI15_10_IRQHandler() {
	// user button
	EXTI->PR1 |= EXTI_PR1_PIF13;
	buzzer_off();
}
void SysTick_Handler() {
	timer = (timer+1)%1000; // loop for 1 sec.
	if(!timer && countdown) {
		countdown --;
	}
}



int keypad_scan() {
	int key;
	int flag_keypad, flag_debounce, flag_key;
	int table[4][4] = {{1, 4, 7, 15}, {2, 5, 8, 0}, {3, 6, 9, 14}, {10, 11, 12, 13}};
	int i_pin_offset[4] = {11, 10, 15, 14};
	int o_pin_offset[4] = {1, 2, 5, 3};
	int i, j;

	while(1) {
		GPIOB->ODR |= 0x002E;
		flag_keypad = GPIOB->IDR & 0xCC00; // PB11, 10, 15, 14
		if(flag_keypad != 0){ // detected input -> debounce
			int k = 100;
			while(k!=0) {
				flag_debounce = GPIOB->IDR & 0xCC00;
				k --;
			}
			if(flag_debounce != 0) {
				for(i=0; i<4; i++) { // scan keypad columns (output)
					// set all outputs low excepts the test col.
					GPIOB->ODR = 0xFFFF0000 | (1<<o_pin_offset[i]);

					for(j=0; j<4; j++) { // read keypad rows (input)
						flag_key = GPIOB->IDR & (1<<i_pin_offset[j]);
						if(flag_key != 0){
							key = table[i][j];
						}
					}
				}
				return key;
			}
		}
		return 0;
	}
}

void buzzer_on() {
	GPIOA->BSRR |= 1<<5;
	TIM3->CR1 |= TIM_CR1_CEN;
	SYST_CSR &= 0xFFFFFFF8;
}

void buzzer_off() {
	GPIOA->BRR |= 1<<5;
	TIM3->CR1 &= ~TIM_CR1_CEN;
	SYST_CSR |= 0x7;
}

