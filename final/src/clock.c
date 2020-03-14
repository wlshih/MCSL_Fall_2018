#include "scoreboard.h"

void clock_init() { // PA5, 6, 7, 0, 1, 4
	RCC->AHB2ENR |= (RCC_AHB2ENR_GPIOAEN);

	// game clock
	SET_REG(GPIOA->MODER, (GPIO_MODER_MODE5 | GPIO_MODER_MODE6 | GPIO_MODER_MODE7), 0x15<<10); // output
	gameclock_send(DECODE_MODE, 0xFF);
	gameclock_send(INTENSITY, 0xA);
	gameclock_send(SCAN_LIMIT, 4);
	gameclock_send(SHUTDOWN, 1);
	gameclock_send(DISPLAY_TEST, 0);

	gameclock_send(1, 0x0); // display "0"
	gameclock_send(2, 0x0); // display "0"
	gameclock_send(3, 0xF); // display " "
	gameclock_send(4, 0x0); // display "0"
	gameclock_send(5, 0x0); // display "0"

	// shot clock
	SET_REG(GPIOA->MODER, (GPIO_MODER_MODE0 | GPIO_MODER_MODE1 | GPIO_MODER_MODE4), 0x105); // output
	shotclock_send(DECODE_MODE, 0xFF);
	shotclock_send(INTENSITY, 0xA);
	shotclock_send(SCAN_LIMIT, 1);
	shotclock_send(SHUTDOWN, 1);
	shotclock_send(DISPLAY_TEST, 0);

//	shotclock_send(1, 0x4); // display "4"
//	shotclock_send(2, 0x2); // display "2"
}


void gameclock_display(int min, int sec, int frac) {
	// final count down
	if(min == 0) {
		if((sec/10) == 0) {
			gameclock_send(5, 0xF); // display " "
		}
		else {
			gameclock_send(5, (sec/10)); // display "N"
		}
		gameclock_send(4, (sec%10)+128); // display "N"
		gameclock_send(3, (frac/10)); // display "n"
		gameclock_send(2, (frac%10)); // display "n"
		gameclock_send(1, 0xF); // display" "
	}
	else {
		if((min/10) == 0) {
			gameclock_send(5, 0xF); // display " "
		}
		else {
			gameclock_send(5, (min/10)); // display "M"
		}
		gameclock_send(4, (min%10)); // display "M"
		gameclock_send(3, 0xF); // display " "
		gameclock_send(2, (sec/10));
		gameclock_send(1, (sec%10));
	}
}

void shotclock_display(int sec, int frac) {
	// count down
	if(sec < 10) {
		shotclock_send(2, sec+128);
		shotclock_send(1, frac/10);
	}
	else {
		shotclock_send(2, (sec/10));
		shotclock_send(1, (sec%10));
	}
}

void gameclock_send(int addr, int data) {
	max7219_send(addr, data, 0x0020, 0x0040, 0x0080);
}

void shotclock_send(int addr, int data) {
	max7219_send(addr, data, 0x0001, 0x0002, 0x0010);
}

void max7219_send(int addr, int data, int DIN, int CS, int CLK) {
	int serial = (addr << 8) | data;
	for(int i=15; i>=0; i--) {
		GPIOA->BRR |= CLK;
		if(serial & (1<<i)) {
			GPIOA->BSRR |= DIN;
		}
		else {
			GPIOA->BRR |= DIN;
		}
		GPIOA->BSRR |= CLK;
	}
	GPIOA->BSRR |= CS;
	GPIOA->BRR |= CS;
}

void gameclock_update() {
	int sec_round = 0;
	int min_round = 0;
	//int a = gameclk[0], b =gameclk[1], c = gameclk[2];

	if(pause) return;
	// clock is running
	if((TIM3->CNT % 10) == 0) {
		gameclk[2] = (gameclk[2]+99) % 100;
		if(gameclk[2] == 99) {
			sec_round = 1;
		}
		else {
			sec_round = 0;
		}
		if(sec_round) {
			gameclk[1] = (gameclk[1]+59) % 60;
			if(gameclk[1] == 59) {
				min_round = 1;
			}
			else {
				min_round = 0;
			}
			if(min_round) {
				gameclk[0] = (gameclk[0]+99) % 100;
			}
		}
		if(gameclk[0] == 0 && gameclk[1] == 0 && gameclk[2] == 0) {
			time_pause();
			buzzer_long();
			sclk_flag = 100*60*100;
		}
		gameclock_display(gameclk[0], gameclk[1], gameclk[2]);
	}
}

void shotclock_update() { // based on gameclock
	//shotclock_display(0, TIM4->CNT/100);
	//int a = shotclk[0], b =shotclk[1];

	int delta = sclk_flag - (gameclk[0]*60*100 + gameclk[1]*100 + gameclk[2]);
	int sec = delta / 100;
	int frac = delta % 100;

	shotclk[0] = 24 - (sec%25);
	shotclk[1] = 100 - frac;
// final count down
	if(gameclk[0] == 0 && (shotclk[0] > gameclk[1] || (shotclk[0] == gameclk[1] && shotclk[1] >= gameclk[2]))) {
		shotclk[0] = gameclk[1];
		shotclk[1] = gameclk[2];
	}
// 24 sec
	else if(!setup && shotclk[0] == 0 && shotclk[1] == 1) {
		buzzer_short();
	}
	shotclock_display(shotclk[0], shotclk[1]);
}

void time_start() {
	pause = 0;
	TIM3->CNT = gclk_flag;
}

void time_pause() {
	pause = 1;
	gclk_flag = TIM3->CNT;
	//sclk_flag = TIM4->CNT;
}

void shotclk_reset() {
	shotclk[0] = 24;
	shotclk[1] = 99;
	sclk_flag = gameclk[0]*60*100 + gameclk[1]*100 + gameclk[2];
	//TIM4->CNT = sclk_flag;

	shotclock_display(shotclk[0], shotclk[1]);
}

void gameclk_add_1min() {
	if(gameclk[0] < 99)	{
		gameclk[0] += 1;
		sclk_flag += 60*100;
	}
	gameclock_display(gameclk[0], gameclk[1], gameclk[2]);
}

void gameclk_sub_1min() {
	if(gameclk[0] > 0)	{
		gameclk[0] -= 1;
		sclk_flag -= 60*100;
	}
	gameclock_display(gameclk[0], gameclk[1], gameclk[2]);
}

void shotclk_add_1sec() {
	if(shotclk[0] < 24) sclk_flag -= 100;
	shotclock_display(shotclk[0], shotclk[1]);
}

void shotclk_sub_1sec() {
	if(shotclk[0] > 0) sclk_flag += 100;
	shotclock_display(shotclk[0], shotclk[1]);
}

