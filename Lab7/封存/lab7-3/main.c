

#include "stm32l476xx.h"
#include "core_cm4.h"

#define PRESS 1
#define NOT_PRESS 0
extern void GPIO_init();



void GPIO_init_AF(){
//TODO: Initial GPIO pin as alternate function for buzzer. You can choose to use C or assembly to finish this function.
	//PB3 TIM2 CH2 AF1=0001
	GPIOB->AFR[0]=GPIOB->AFR[0]|(0b0001<<12);
}

void Timer_init(){
//TODO: Initialize timer
	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;
	TIM2->CR1=TIM2->CR1&0xFF8F;
	TIM2->ARR = (uint32_t)100;//Reload value
	TIM2->EGR = TIM2->EGR|0b1;//Reinitialize the counter
}

void PWM_channel_init(){
//TODO: Initialize timer PWM channel
	TIM2->CCMR1=TIM2->CCMR1|(0b1101000<<8);
	TIM2->CCR2 = (uint32_t)50;
	TIM2->CCR1 = (uint32_t)50;
	TIM2->CCER|=(0b1<<4);
}

void keypad_init()
{
	// SET keypad GPIO OUTPUT //
	RCC->AHB2ENR = RCC->AHB2ENR|0b101;
	//Set PA8,9,10,12 as output mode
	GPIOA->MODER= GPIOA->MODER&0xFDD5FFFF;
	//set PA8,9,10,12 is Pull-up output
	GPIOA->PUPDR=GPIOA->PUPDR|0x1150000;
	//Set PA8,9,10,12 as medium speed mode
	GPIOA->OSPEEDR=GPIOA->OSPEEDR|0x1150000;
	//Set PA8,9,10,12 as high
	GPIOA->ODR=GPIOA->ODR|10111<<8;

	// SET keypad GPIO INPUT //
	//Set PC5,6,7,9 as INPUT mode
	GPIOC->MODER=GPIOC->MODER&0xFFF303FF;
	//set PC5,6,7,9 is Pull-down input
	GPIOC->PUPDR=GPIOC->PUPDR|0x8A800;
	//Set PC5,6,7,9 as medium speed mode
	GPIOC->OSPEEDR=GPIOC->OSPEEDR|0x45400;
}

int main(){
	GPIO_init();
	GPIO_init_AF();
	Timer_init();
	keypad_init();
	PWM_channel_init();
	//TODO: Scan the keypad and use PWM to send the corresponding frequency
	int col, row;
	int press_stat, before_val=-1, press_val;
	int Table[4][4]={153, 115, 81, 1, 136, 102, 76, 0, 121, 91, 0, 2, 0, 0, 0, 0};
	int por=50;
	int por2=50;
	while(1){
		for(col=0; col<4; col++){
			int temp_c=col;
			if(col==3) temp_c++;
			GPIOA->ODR = (GPIOA->ODR & 0xFFFFE8FF)|(1<<(temp_c+8));
			for(row = 0; row < 4; row++){
				int temp_r=row;
				if(row==3) temp_r++;
				if((GPIOC->IDR) & (1<<(5+temp_r))){
					if(Table[col][row]==1){
						press_stat = PRESS;
						if(before_val!=1){
							press_val=1;
							por+=5;
							por2-=5;
							if(por>90){
								por=90;
								por2=10;
							}
							TIM2->CCR1 = (uint32_t)por;
							TIM2->CCR2 = (uint32_t)por2;

						}
					}else if(Table[col][row]==2){
						press_stat = PRESS;
						if(before_val!=2){
							press_val=2;
							por-=5;
							por2+=5;
							if(por2>90){
								por=10;
								por2=90;
							}
							TIM2->CCR1 = (uint32_t)por;
							TIM2->CCR2 = (uint32_t)por2;
						}
					}else if(Table[col][row]!=0){
						press_stat = PRESS;
						if(before_val != -1){
							//TIM2->EGR = TIM2->EGR|0b1;
							TIM2->CR1 |= TIM_CR1_CEN; //enable
						}
						if(before_val != Table[col][row]){
							press_val = Table[col][row];
							TIM2->PSC = Table[col][row];
						}
					}
				}
			}
		}
		if(press_stat == PRESS) before_val = press_val;
		else{
			TIM2->CR1 &= ~TIM_CR1_CEN;
			before_val = -1;
		}
		press_stat = NOT_PRESS;
	}
}
