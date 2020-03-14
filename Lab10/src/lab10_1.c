//#include "stm32l476xx.h"
//#define SET_REG(REG,SELECT,VAL) {((REG)=((REG)&(~(SELECT))) | (VAL));};
//
//void GPIO_init();
//void USART3_init();
//int UART_Transmit(uint8_t* arr, uint32_t size);
//int button_pressed();
//
//uint8_t* s = "Hello world!!\n\r";
//uint8_t test = 6+'0';
//
//int main() {
//	GPIO_init();
//	USART3_init();
//	while(1) {
//		if(button_pressed()) {
//			GPIOA->BSRR |= GPIO_BSRR_BS5;
//			while(button_pressed()); // wait for button release
//			GPIOA->BRR |= GPIO_BRR_BR5;
//			UART_Transmit(s, 15);
//		}
//		else GPIOA->BRR |= GPIO_BRR_BR5;
//	}
//
//	return 0;
//}
//
//
//void GPIO_init() {
//	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN | RCC_AHB2ENR_GPIOAEN;
//
//	// usart, pc10 for rx, pc11 for tx
//	SET_REG(GPIOC->MODER, GPIO_MODER_MODE10, (uint32_t)2<<20);
//	SET_REG(GPIOC->OTYPER, GPIO_OTYPER_OT10, 0);
//	SET_REG(GPIOC->AFR[1], GPIO_AFRH_AFSEL10, (uint32_t)7<<8);
//
//	SET_REG(GPIOC->MODER, GPIO_MODER_MODE11, (uint32_t)2<<22);
//	SET_REG(GPIOC->OTYPER, GPIO_OTYPER_OT11, 0);
//	SET_REG(GPIOC->AFR[1], GPIO_AFRH_AFSEL11, (uint32_t)7<<12);
//	// button
//	SET_REG(GPIOC->MODER, GPIO_MODER_MODE13, 0);
//	SET_REG(GPIOC->OTYPER, GPIO_OTYPER_OT13, 0);
//	SET_REG(GPIOC->PUPDR, GPIO_PUPDR_PUPD13, (uint32_t)2<<26);
//	// ld2, for test
//	SET_REG(GPIOA->MODER, GPIO_MODER_MODE5, (uint32_t)1<<10);
//	SET_REG(GPIOA->OSPEEDR, GPIO_OSPEEDR_OSPEED5, (uint32_t)2<<10);
//
//	GPIOA->BSRR |= GPIO_BSRR_BS5;
//}
//
//void USART3_init() {
//	RCC->APB1ENR1 |= RCC_APB1ENR1_USART3EN;
//
//	// CR1
//	SET_REG(USART3->CR1, (USART_CR1_TE | USART_CR1_RE), 0xC);
//	SET_REG(USART3->CR1, (USART_CR1_M | USART_CR1_PS | USART_CR1_PCE | USART_CR1_OVER8), 0);
//	// CR2
//	SET_REG(USART3->CR2, USART_CR2_STOP, 0); // 1-bit stop
//	// CR3
//	SET_REG(USART3->CR3, (USART_CR3_RTSE | USART_CR3_CTSE | USART_CR3_ONEBIT), 0);
//	SET_REG(USART3->BRR, 0xFF, 4000000/9600);
//
//	SET_REG(USART3->CR2, (USART_CR2_LINEN | USART_CR2_CLKEN), 0);
//	SET_REG(USART3->CR3, (USART_CR3_SCEN | USART_CR3_HDSEL | USART_CR3_IREN), 0);
//
//	// enable usart
//	USART3->CR1 |= USART_CR1_UE;
//}
//
//int UART_Transmit(uint8_t *arr, uint32_t size) {
//	for(int i=0; i<size; i++) {
//		while(~(USART3->ISR) & USART_ISR_TC); // TC == 0, wait
//			USART3->TDR = arr[i];
//	}
//	while(~(USART3->ISR) & USART_ISR_TC); // TC == 0, wait
//
//	return 1; // transmit finished
//}
//
//int button_pressed() {
//	int debounce = 10000;
//	int flag;
//	if(GPIOC->IDR & GPIO_IDR_ID13) return 0;
//	while(debounce != 0) {
//		flag = (GPIOC->IDR & GPIO_IDR_ID13);
//		debounce--;
//	}
//	return flag ? 0 : 1;
//}
