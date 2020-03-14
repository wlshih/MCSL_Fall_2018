	.syntax unified
	.cpu cortex-m4
	.thumb

.text
	.global GPIO_init

	.equ RCC_AHB2ENR, 0x4002104C
	.equ GPIOB_MODER, 0x48000400
	.equ GPIOB_PUPDR, 0x4800040C
	.equ GPIOB_ODR, 0x48000414


GPIO_init:
	push {r0, r1, r2, lr}

	//Enable AHB2 clock B
	movs r0, #0x2
	ldr r1, =RCC_AHB2ENR
	str r0, [r1]

	//set PB3 output mode
	ldr r0, =#0b10000000 //0b 0100 0000
	ldr r1, =GPIOB_MODER
	ldr r2, [r1]
	and r2, 0xFFFFFF3F //FFFFFF3F
	orrs r2, r2, r0
	str r2, [r1]

	//set PB3,PB4,PB5 as high speed mode
	//ldr r1, =GPIOB_OSPEEDR
	//ldr r0, =#0xA80 //101010000000
	//str r0, [r1]

	pop {r0, r1, r2, pc}
