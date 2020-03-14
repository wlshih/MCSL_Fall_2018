#include "stm32l476xx.h"

#define SET_REG(REG,SELECT,VAL) {((REG)=((REG)&(~(SELECT))) | (VAL));};

// max7219 address
#define DECODE_MODE 0x9
#define INTENSITY 0xA
#define SCAN_LIMIT 0xB
#define SHUTDOWN 0xC
#define DISPLAY_TEST 0xF

extern int time;
extern int key_press;
extern int pause;
extern int gameclk[3];
extern int gclk_flag;
extern int shotclk[2];
extern int sclk_flag;
extern int quarter;
extern int buzz;
extern int setup;

void GPIO_init();
void keypad_init();
int keypad_scan();
void keypad_instr(int key);

void timer_init();
void clock_init();
void gameclock_display(int min, int sec, int frac);
void shotclock_display(int sec, int frac);
void gameclock_send(int addr, int data);
void shotclock_send(int addr, int data);
void max7219_send(int addr, int data, int DIN, int CS, int CLK);

void gameclock_update();
void shotclock_update();
void time_start();
void time_pause();

void keypad_instr(int key);
void gameclk_add_1min();
void gameclk_sub_1min();
void shotclk_add_1sec();
void shotclk_sub_1sec();
void shotclk_reset();


void score_init();
void score_host_add();
void score_host_sub();
void score_guest_add();
void score_guest_sub();
void next_quarter();

void buzzer_init();
void buzzer_update();
//void buzzer_send();
void buzzer_long();
void buzzer_short();

void reset();
