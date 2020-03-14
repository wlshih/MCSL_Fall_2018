/*
extern void GPIO_init();
extern void max7219_init();
extern void max7219_send(unsigned int address, unsigned int data);

void display(int* id, int num_digs) {
	int i = 0;
	max7219_send(0xB, num_digs-1); // set scan limit
	for(i=0; i<num_digs; i++) {
		max7219_send(num_digs-i, id[i]);
	}
}

int main() {
	int id[7] = {0, 5, 1, 6, 0, 7, 6};
	GPIO_init();
	max7219_init();
	display(id, 7);

	return 0;
}

*/
