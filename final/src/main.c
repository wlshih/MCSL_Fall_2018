#include "scoreboard.h"

int time = 0;
int key_press = 0;
int pause = 1;

int gameclk[3] = { 0, 0, 0 };
int gclk_flag = 0;
int shotclk[2] = { 24, 99 };
int sclk_flag = 100*60*100;

int quarter = 0;
int buzz = 0;

int setup = 1;


int main() {
 	keypad_init();
	timer_init();
	clock_init();
	score_init();
	buzzer_init();
	SysTick_Config(4000); // 1ms

	//gameclock_display(0, 20, 55);
	//shotclock_display(21, 5);

	while(1) {
		int key = keypad_scan();
		keypad_instr(key);
		shotclock_update();
		gameclock_update();
		buzzer_update();
	}

	return 0;
}

void reset() {
	time = 0;
	key_press = 0;
	pause = 1;

	gameclk[0] = 0;
	gameclk[1] = 0;
	gameclk[2] = 0;
	gclk_flag = 0;
	shotclk[0] = 24;
	shotclk[1] = 99;
	sclk_flag = 100*60*100;

	quarter = 0;
	buzz = 0;

	setup = 1;

	GPIOB->BSRR |= 1;

	GPIOC->BSRR|= 1;
	GPIOC->BSRR|= 1<<1;
	GPIOC->BSRR|= 1<<2;
	GPIOC->BSRR|= 1<<3;

	gameclock_send(1, 0x0); // display "0"
	gameclock_send(2, 0x0); // display "0"
	gameclock_send(3, 0xF); // display " "
	gameclock_send(4, 0x0); // display "0"
	gameclock_send(5, 0x0); // display "0"
}

void SysTick_Handler() {

}
