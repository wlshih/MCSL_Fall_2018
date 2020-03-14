/*
#include <stm32l476xx.h>
//TODO: define your gpio pin
#define X0 13
#define X1 14
#define X2 15
#define X3 1
#define Y0 3
#define Y1 5
#define Y2 4
#define Y3 10

#define SCAN_LIMIT 0xB

//unsigned int x_pin[4] = {X0, X1, X2, X3};
//unsigned int y_pin[4] = {Y0, Y1, Y2, Y3};

extern void GPIO_init();
extern void max7219_init();
extern void max7219_send(unsigned int address, unsigned int data);

void keypad_init();
int keypad_scan(); // return keypad value
void display(int);


int main() {

	GPIO_init();
	max7219_init();
	keypad_init();

	while(1) {
		display(keypad_scan());
	}
	//display(12345678);
	return 0;
}

void keypad_init() {
	// enable GPIOB clock
	RCC->AHB2ENR |= 0x2;

	// GPIO output (PB1, PB15, PB14, PB13)
	GPIOB->MODER &= 0x57FFFFF7; // output 0x01
	GPIOB->PUPDR |= 0x54000004; // pull-up 0x01
	GPIOB->OSPEEDR |= 0x54000004; // mid-speed 0x01

	// GPIO input (PB10, PB4, PB5, PB3)
	GPIOB->MODER &= 0xFFCFF03F; // input 0x00
	GPIOB->PUPDR &= 0; // pull-down 0x10
	GPIOB->PUPDR |= 0x00202A80;
	GPIOB->OSPEEDR |= 0x54000004; // mid-speed 0x01
}

// return keypad value
int keypad_scan() {
	int key;
	int flag_keypad, flag_debounce, flag_key;
	int table[4][4] = {{1, 4, 7, 15}, {2, 5, 8, 0}, {3, 6, 9, 14}, {10, 11, 12, 13}};
	int o_pin_offset[4] = {13, 14, 15, 1};
	int i_pin_offset[4] = {3, 5, 4, 10};
	int i, j;

	while(1) {
		GPIOB->ODR |= 0xE002;
		flag_keypad = GPIOB->IDR & 0x0438; // PB10, 4, 5, 3
		if(flag_keypad != 0){ // detected input -> debounce
			int k = 45000;
			while(k!=0) {
				flag_debounce = GPIOB->IDR & 0x0438;
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

void display(int num) {
	int arr[8];
	int num_digs = 0;

	if(num == 0) {
		max7219_send(SCAN_LIMIT, 0);
		max7219_send(1, 0);
	}

	while(num != 0) {
		arr[num_digs] = (num%10);
		num /= 10;
		num_digs ++;
	}
	max7219_send(SCAN_LIMIT, num_digs-1);
	while(num_digs != 0) {
		max7219_send(num_digs, arr[num_digs-1]);
		num_digs --;
	}
}

*/
