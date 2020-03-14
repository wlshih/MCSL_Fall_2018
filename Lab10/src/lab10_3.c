#include "stm32l476xx.h"
#include <string.h>
#include <stdlib.h>
#define SET_REG(REG,SELECT,VAL) {((REG)=((REG)&(~(SELECT))) | (VAL));};

void GPIO_init();
void USART3_init();
int UART_Transmit(char* arr);
int UART_Receive(char* arr);

void ADC_init();
void TIM3_init();

void EXTI_config();
void NVIC_config();

int button_pressed();
void delay();

int time = 0;
int mode = 0; // 0: input, (1: show id), 2: light, (3: led on, 4: led off)
int led = 0;


char cmd[100];
int value = 0;
char buf[10]; // resistor value
char* sid = "0516076\r\n";

int main() {
	GPIO_init();
	USART3_init();
	TIM3_init();
	ADC_init();
	SysTick_Config(4000); // 1 ms
	EXTI_config();
	NVIC_config();

	UART_Transmit("\r\n> ");
	while(1); // wait for interrupts

	return 0;
}

void USART3_IRQHandler() {
	char tmp[2];
	UART_Receive(tmp);

	if(mode == 0) { // input mode
		if(tmp[0] == '\r') { // enter
			UART_Transmit("\r\n");
//			UART_Transmit(cmd);
			if(strcmp(cmd, "showid") == 0) {
				UART_Transmit(sid);
			}
			else if(strcmp(cmd, "light") == 0) {
				mode = 2;
			}
			else if(strcmp(cmd, "led on") == 0) {
				GPIOA->BSRR |= 1<<5;
				led = 1;
			}
			else if(strcmp(cmd, "led off") == 0) {
				GPIOA->BRR |= 1<<5;
				led = 0;
			}
			else if(strcmp(cmd, "led status") == 0) {
				if(led)	UART_Transmit("led on\r\n");
				else UART_Transmit("led off\r\n");
			}
			else if(strlen(cmd) > 0) {
				UART_Transmit("Unknown Command\r\n");
			}
			memset(cmd, 0, sizeof(cmd));
			if(mode != 2) UART_Transmit("> ");
		}
		else if(tmp[0] == 127) { // i don't know why my backspace == 127
			if(strlen(cmd) > 0) {
				UART_Transmit("\177");
				cmd[strlen(cmd)-1] = '\0';
			}
		}
		else {
			int deb = tmp[0];
			UART_Transmit(tmp);
			cmd[strlen(cmd)] = tmp[0];
		}
	}
	else if(mode == 2 && (tmp[0] == 'q' || tmp[0] == 'Q')) {
		mode = 0;
		UART_Transmit("> ");
	}
}


void GPIO_init() {
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN | RCC_AHB2ENR_GPIOAEN;

	// usart, pc10 for rx, pc11 for tx
	SET_REG(GPIOC->MODER, GPIO_MODER_MODE10, (uint32_t)2<<20);
	SET_REG(GPIOC->OTYPER, GPIO_OTYPER_OT10, 0);
	SET_REG(GPIOC->AFR[1], GPIO_AFRH_AFSEL10, (uint32_t)7<<8);

	SET_REG(GPIOC->MODER, GPIO_MODER_MODE11, (uint32_t)2<<22);
	SET_REG(GPIOC->OTYPER, GPIO_OTYPER_OT11, 0);
	SET_REG(GPIOC->AFR[1], GPIO_AFRH_AFSEL11, (uint32_t)7<<12);

	// button
	SET_REG(GPIOC->MODER, GPIO_MODER_MODE13, 0);
	SET_REG(GPIOC->OTYPER, GPIO_OTYPER_OT13, 0);
	SET_REG(GPIOC->PUPDR, GPIO_PUPDR_PUPD13, (uint32_t)2<<26);

	// ld2
	SET_REG(GPIOA->MODER, GPIO_MODER_MODE5, (uint32_t)1<<10);
	SET_REG(GPIOA->OSPEEDR, GPIO_OSPEEDR_OSPEED5, (uint32_t)2<<10);

	// photoresistor, pc0
	SET_REG(GPIOC->MODER, GPIO_MODER_MODE0, 0b11); // analog mode
	GPIOC->ASCR |= GPIO_ASCR_ASC0; // analog switch control reg

//	GPIOA->BSRR |= GPIO_BSRR_BS5;
}

void USART3_init() {
	RCC->APB1ENR1 |= RCC_APB1ENR1_USART3EN;

	// CR1
	SET_REG(USART3->CR1, (USART_CR1_TE | USART_CR1_RE), 0xC); // enable tx rx
	SET_REG(USART3->CR1, (USART_CR1_M | USART_CR1_PS | USART_CR1_PCE | USART_CR1_OVER8), 0);
	SET_REG(USART3->CR1, USART_CR1_RXNEIE, USART_CR1_RXNEIE); // enable RXNE interrupt
	// CR2
	SET_REG(USART3->CR2, USART_CR2_STOP, 0); // 1-bit stop
	// CR3
	SET_REG(USART3->CR3, (USART_CR3_RTSE | USART_CR3_CTSE | USART_CR3_ONEBIT), 0);
	SET_REG(USART3->BRR, 0xFF, 4000000/9600); // (freq / baude_rate)

	// must be kept cleared in asynchronous mode
	SET_REG(USART3->CR2, (USART_CR2_LINEN | USART_CR2_CLKEN), 0);
	SET_REG(USART3->CR3, (USART_CR3_SCEN | USART_CR3_HDSEL | USART_CR3_IREN), 0);

	// enable usart
	USART3->CR1 |= USART_CR1_UE;
}

int UART_Transmit(char *arr) {
	int size = strlen(arr);
	for(int i=0; i<size; i++) {
		while(~(USART3->ISR) & USART_ISR_TC); // TC == 0, wait
			USART3->TDR = arr[i];
	}
	while(~(USART3->ISR) & USART_ISR_TC); // TC == 0, wait

	return 1; // transmit finished
}

int UART_Receive(char *arr) {
	// receive 1 character from UART
	arr[0] = USART3->RDR;
	arr[1] = '\0';
	return 1;
}

void ADC_init() {
	RCC->AHB2ENR |= RCC_AHB2ENR_ADCEN;
	// disable deep-power-down mode
	SET_REG(ADC1->CR, ADC_CR_DEEPPWD, 0);
	// enable voltage regulator;
	ADC1->CR |= ADC_CR_ADVREGEN;

	delay();

	SET_REG(ADC123_COMMON->CCR, ADC_CCR_CKMODE, ADC_CCR_CKMODE_0);
	SET_REG(ADC123_COMMON->CCR, ADC_CCR_PRESC, 0) // not prescaled
	SET_REG(ADC1->CFGR, ADC_CFGR_CONT, ADC_CFGR_CONT); // continuous mode
	SET_REG(ADC1->CFGR, ADC_CFGR_EXTEN, ADC_CFGR_EXTEN_0); // external rising edge
	SET_REG(ADC1->CFGR, ADC_CFGR_EXTSEL, ADC_CFGR_EXTSEL_2); // external event 4: TIM3
	SET_REG(ADC1->CFGR, ADC_CFGR_ALIGN, 0); // right alignment
	SET_REG(ADC1->CFGR, ADC_CFGR_RES, 0); // 12-bit resolution

	SET_REG(ADC1->SQR1, ADC_SQR1_L, 0); // sequence length 1
	SET_REG(ADC1->SQR1, ADC_SQR1_SQ1, 1<<6) // channel 1 for pc0

	SET_REG(ADC1->SMPR1, ADC_SMPR1_SMP1, 1<<3) // sampling time 6.5 adc blocks

	// enable the End-Of-Conversion Interrupt
	ADC1->IER |= ADC_IER_EOCIE;
	// enable the ADC
	ADC1->CR |= ADC_CR_ADEN;

	while (!(ADC1->ISR & ADC_ISR_ADRDY)); // wait until adc is ready
	ADC1->CR |= ADC_CR_ADSTART; // start adc
}

void TIM3_init() {
	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM3EN;

	SET_REG(TIM3->CR1, TIM_CR1_CEN, TIM_CR1_CEN);
	SET_REG(TIM3->CR1, TIM_CR1_DIR, TIM_CR1_DIR); // downcount
	SET_REG(TIM3->CR2, TIM_CR2_MMS, TIM_CR2_MMS_1); // the update event is selected as trigger output(TRGO)
	SET_REG(TIM3->EGR, TIM_EGR_UG, TIM_EGR_UG);
	TIM3->PSC = 3999;
	TIM3->ARR = 999; // 1ms
}

void EXTI_config() {
	// clock enable
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

	// manage the external interrupt to GPIOs
	SYSCFG->EXTICR[3] |= SYSCFG_EXTICR4_EXTI13_PC;

	// set up the interrupt mask register EXTI_IMR1
	EXTI->IMR1 |= EXTI_IMR1_IM13;

	// set the rising-triggered interrupt
	EXTI->RTSR1 |= EXTI_RTSR1_RT13;
}

void NVIC_config() {
	NVIC_SetPriority(USART3_IRQn, 0); // high priority
	NVIC_SetPriority(SysTick_IRQn, 1);
	NVIC_SetPriority(ADC1_2_IRQn, 2);
	NVIC_EnableIRQ(USART3_IRQn);
	NVIC_EnableIRQ(ADC1_2_IRQn);
}

void itos(int num) {
	int len, tmp = num;
	for (len = 0; tmp > 0; len++) tmp /= 10;
	for (int i = len - 1; i >= 0; i--) {
		buf[i] = num % 10 + '0';
		num /= 10;
	}
	buf[len] = '\r';
	buf[len+1] = '\n';

//	return str;
}

int button_pressed() {
	int debounce = 10000;
	int flag;
	if(GPIOC->IDR & GPIO_IDR_ID13) return 0;
	while(debounce != 0) {
		flag = (GPIOC->IDR & GPIO_IDR_ID13);
		debounce--;
	}
	return flag ? 0 : 1;
}

void delay() {
	for(int i=0; i<100; i++);
}

//void EXTI15_10_IRQHandler() {
//	while(button_pressed());
//	itos(value);
//	UART_Transmit(buf);
//
//	// clear pending, or interrupt will be triggered continually
//	EXTI->PR1 |= EXTI_PR1_PIF13;
//}

void SysTick_Handler() {
	time = (time + 1) % 500;
	if(mode == 2 && time == 0) {
		itos(value);
		UART_Transmit(buf);
	}
}

void ADC1_2_IRQHandler() {
	value = ADC1->DR;
//	int deb = value;

	// End of conversion flag, set by hardware, cleared by software
	ADC1->ISR |= ADC_ISR_EOC;
}
