/*#include "stm32l476xx.h"
#define SET_REG(REG,SELECT,VAL) {((REG)=((REG)&(~(SELECT))) | (VAL));};

extern void delay(int);
void GPIO_init();
void SysTick_init();
void System_config();
void EXTI_config();
void NVIC_config();
int keypad_scan();

int key_input = 0;
int timer = 0;

int main() {
	GPIO_init();
	SysTick_init();
	EXTI_config();
	System_config();
	NVIC_config();
	while(1) {
		//EXTI15_10_IRQHandler();
	}
}
void System_config() {
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

	SYSCFG->EXTICR[2] |= SYSCFG_EXTICR3_EXTI10_PB;
	SYSCFG->EXTICR[3] |= SYSCFG_EXTICR4_EXTI15_PB;
	SYSCFG->EXTICR[3] |= SYSCFG_EXTICR4_EXTI14_PB;
	SYSCFG->EXTICR[2] |= SYSCFG_EXTICR3_EXTI11_PB;
}
void EXTI_config() {
	EXTI->IMR1 |= (EXTI_IMR1_IM10 | EXTI_IMR1_IM15 | EXTI_IMR1_IM14 | EXTI_IMR1_IM11);
	EXTI->RTSR1 |= (EXTI_RTSR1_RT10 | EXTI_RTSR1_RT15 | EXTI_RTSR1_RT14 | EXTI_RTSR1_RT11);
	//EXTI->PR1 |= (EXTI_PR1_PIF10 | EXTI_PR1_PIF15 | EXTI_PR1_PIF14 | EXTI_PR1_PIF11);
}
// enable LD2 on PA5
// keypad input on PB10, 15, 14, 11
// keypad output on PB1, 4, 5, 3
void GPIO_init() {
	RCC->AHB2ENR |= (RCC_AHB2ENR_GPIOAEN | RCC_AHB2ENR_GPIOBEN);
	// LED
	SET_REG(GPIOA->MODER, GPIO_MODER_MODE5, (uint32_t)1<<10);
	SET_REG(GPIOA->OSPEEDR, GPIO_OSPEEDR_OSPEED5, (uint32_t)2<<10);
	// keypad input
	SET_REG(GPIOB->MODER, (GPIO_MODER_MODE10 | GPIO_MODER_MODE15 | GPIO_MODER_MODE14 | GPIO_MODER_MODE11), 0);
	SET_REG(GPIOB->PUPDR, (GPIO_PUPDR_PUPD10 | GPIO_PUPDR_PUPD15 | GPIO_PUPDR_PUPD14 | GPIO_PUPDR_PUPD11), (uint32_t)0xA0A00000u);
	// keypad output
	SET_REG(GPIOB->MODER, (GPIO_MODER_MODE1 | GPIO_MODER_MODE4 | GPIO_MODER_MODE5 | GPIO_MODER_MODE3), (uint32_t)0x00000544u);
	SET_REG(GPIOB->OSPEEDR, (GPIO_OSPEEDR_OSPEED1 | GPIO_OSPEEDR_OSPEED4 | GPIO_OSPEEDR_OSPEED5 | GPIO_OSPEEDR_OSPEED3), (uint32_t)0x00000544u);

	GPIOA->BSRR |= GPIO_BSRR_BS5;
	GPIOB->BSRR |= (GPIO_BSRR_BS10 | GPIO_BSRR_BS15 | GPIO_BSRR_BS14 | GPIO_BSRR_BS11);
}

void SysTick_init() {
	SysTick_Config(4000); // 1ms
	NVIC_SetPriority (SysTick_IRQn, 0); // set highest SysTick priority
}

void NVIC_config() {
	// IRQn = 40
	NVIC->IP[40] |= (uint8_t)0xF0u; // lowest interrupt priority
	NVIC->ISER[1] |= (uint32_t)0x00000100u;
	//NVIC->ISPR[1] |= (uint32_t)0x00000100u;
}

// interrupt handler implementation
void EXTI15_10_IRQHandler() {
	//EXTI->PR1 &= ~(EXTI_PR1_PIF10 & EXTI_PR1_PIF15 & EXTI_PR1_PIF14 & EXTI_PR1_PIF11);
	int n = keypad_scan();
	for(int i=0; i<n; i++) {
		GPIOA->BRR |= GPIO_BRR_BR5;
		timer = 1;
		while(timer != 0);
		GPIOA->BSRR |= GPIO_BSRR_BS5;
		timer = 1;
		while(timer != 0);
	}
	GPIOB->BSRR |= (GPIO_BSRR_BS10 | GPIO_BSRR_BS15 | GPIO_BSRR_BS14 | GPIO_BSRR_BS11);
}
void SysTick_Handler() {
	timer = (timer+1)%200;
}



int keypad_scan() {
	int key;
	int flag_keypad, flag_debounce, flag_key;
	int table[4][4] = {{1, 4, 7, 15}, {2, 5, 8, 0}, {3, 6, 9, 14}, {10, 11, 12, 13}};
	int i_pin_offset[4] = {10, 15, 14, 11};
	int o_pin_offset[4] = {1, 4, 5, 3};
	int i, j;

	while(1) {
		GPIOB->ODR |= 0x003A;
		flag_keypad = GPIOB->IDR & 0xCC00; // PB10, 15, 14, 11
		if(flag_keypad != 0){ // detected input -> debounce
			int k = 45000;
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
	}
}
*/
