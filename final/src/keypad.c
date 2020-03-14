#include "scoreboard.h"

void keypad_init() {
	RCC->AHB2ENR |= (RCC_AHB2ENR_GPIOBEN);

	SET_REG(GPIOB->MODER, (GPIO_MODER_MODE1 | GPIO_MODER_MODE15 | GPIO_MODER_MODE14 | GPIO_MODER_MODE13), 0); // input
	SET_REG(GPIOB->PUPDR, (GPIO_PUPDR_PUPD1 | GPIO_PUPDR_PUPD15 | GPIO_PUPDR_PUPD14 | GPIO_PUPDR_PUPD13), 0XA8000008);
	SET_REG(GPIOB->MODER, (GPIO_MODER_MODE10 | GPIO_MODER_MODE4 | GPIO_MODER_MODE5 | GPIO_MODER_MODE3), 0x00100540); // output
	SET_REG(GPIOB->PUPDR, (GPIO_PUPDR_PUPD10 | GPIO_PUPDR_PUPD4 | GPIO_PUPDR_PUPD5 | GPIO_PUPDR_PUPD3), 0x00100540);
}

int keypad_scan() {
	int key;
	int flag_keypad, flag_debounce, flag_key;
	int table[4][4] = {{1, 4, 7, 15}, {2, 5, 8, 0}, {3, 6, 9, 14}, {10, 11, 12, 13}};
	int i_pin_offset[4] = {1, 15, 14, 13};
	int o_pin_offset[4] = {10, 4, 5, 3};
	int i, j;

	GPIOB->ODR |= 0x0438; // PB10, 4, 5, 3
	flag_keypad = GPIOB->IDR & 0xE002; // PB 1, 15, 14, 13

	if(flag_keypad) { // detected input -> debounce
		int debounce = 100;
		while(debounce!=0) {
			flag_debounce = GPIOB->IDR & 0xE002;
			debounce --;
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
	return -1; // no input detected
}

void keypad_instr(int key) {
	// no input
	if(key < 0) {
		key_press = 0;
		return;
	}
	// redundant signal
	if(key_press) return;

	setup = 0;

	switch(key) {
	case 0:
		if(pause) time_start();
		else time_pause();
		break;
	case 1:
		gameclk_sub_1min();
		break;
	case 3:
		gameclk_add_1min();
		break;
	case 4:
		shotclk_sub_1sec();
		break;
	case 5:
		shotclk_reset();
		break;
	case 6:
		shotclk_add_1sec();
		break;
	case 10:
		score_host_add();
		break;
	case 11:
		score_host_sub();
		break;
	case 12:
		score_guest_add();
		break;
	case 13:
		score_guest_sub();
		break;
	case 14:
		next_quarter();
		break;
	case 15:
		reset();
		break;
	default:
		GPIOB->BRR |= 1;
		break;
	}
	key_press = 1;
}

