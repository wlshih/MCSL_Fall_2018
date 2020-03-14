/*#include "stm32l476xx.h"
#define SET_REG(REG,SELECT,VAL) {((REG)=((REG)&(~(SELECT))) | (VAL));};

extern void delay(int);
void SystemClock_Config();
void GPIO_init();
void LED_switch();

int blink = 0;

int main() {
	//NVIC_EnableIRQ(15);
	SystemClock_Config();
	SysTick_Config(749999); // sysTick reload value 250,000 * 3sec.
	GPIO_init();
	while(1) LED_switch();
}

void SystemClock_Config() {
	RCC->CR |= RCC_CR_HSION; // turn on HSI
	while(RCC->CR & RCC_CR_HSIRDY); // wait until HSI ready
	SET_REG(RCC->CFGR, RCC_CFGR_SW, RCC_CFGR_SW_HSI); // configure HSI as SYSCLK
	SET_REG(RCC->CFGR, RCC_CFGR_SWS, RCC_CFGR_SWS_HSI);
	SET_REG(RCC->CFGR, RCC_CFGR_HPRE, RCC_CFGR_HPRE_DIV64); // SYSCLK divided by 64
}

// enables LD2 on PA5
void GPIO_init() {
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;

	SET_REG(GPIOA->MODER, GPIO_MODER_MODE5, (uint32_t)1<<10);
	SET_REG(GPIOA->OSPEEDR, GPIO_OSPEEDR_OSPEED5, (uint32_t)2<<10);

	GPIOA->BSRR |= GPIO_BSRR_BS5;
}
void LED_switch() {
	if(blink) {
		GPIOA->BRR |= GPIO_BRR_BR5;
		delay(10000);
		GPIOA->BSRR |= GPIO_BSRR_BS5;
		blink = 0;
	}
}

// interrupt handler implementation
void SysTick_Handler() {
	blink = 1;
}
*/
