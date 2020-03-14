/*	.syntax unified
	.cpu cortex-m4
	.thumb

.data
	leds: .byte 0
	button: .byte 0

.text
	.global main
	.equ RCC_AHB2ENR, 0x4002104C

	.equ GPIOB_MODER, 0x48000400
	.equ GPIOB_OTYPER, 0x48000404
	.equ GPIOB_OSPEEDR, 0x48000408
	.equ GPIOB_PUPDR, 0x4800040C
	.equ GPIOB_ODR, 0x48000414

	.equ X, 1000
	.equ Y, 2000

main: // r0 = leds address, r1, r2 = state
	bl GPIO_init

	movs r1, #1
	movs r2, #3
	ldr r0, =leds
	strb r1, [r0]

Loop: // r4 = flag
	bl DisplayLED
	bl Delay

	cmp r1, #8 // Set the direction flag
	it eq
	moveq r4, #0
	cmp r1, #1
	it eq
	moveq r4, #1

	cmp r4, #0 // determine the direction to shift
	ite ne
	lslne r2, #1 // Shift left
	lsreq r2, #1 // Shift right

	mov r1, r2
	lsl r1, #27
	lsr r1, #28
	strb r1, [r0]

	b Loop

GPIO_init:
	// Enable AHB2 GPIOB&C clock
	movs r0, #0x2
	ldr r1, =RCC_AHB2ENR
	str r0, [r1]

	// Set PB3~6 as output mode
	movs r0, #(0x55<<6) // 0x01
	ldr r1, =GPIOB_MODER
	ldr r2, [r1]
	and r2, #0xFFFFC03F // Mask MODERs
	orrs r2, r2, r0
	str r2, [r1]

	// Set PB3~6 asOD output
	movs r0, #(0x<<3) // 0x0
	ldr r1, =GPIOB_OTYPER
	ldr r2, [r1]
	and r2, #0xFFFFFF87 // Mask MODERs
	orrs r2, r2, r0
	str r2, [r1]

	// Set PB3~6 as high speed mode
	movs r0, #(0xAA<<6) // 0x10
	ldr r1, =GPIOB_OSPEEDR
	ldr r2, [r1]
	and r2, #0xFFFFC03F // Mask MODERs
	orrs r2, r2, r0
	str r2, [r1]

	// Set PA3~6 as pull-up
	movs r0, #(0xAA<<6) // 0x10
	ldr r1, = GPIOB_PUPDR
	ldr r2, [r1]
	and r2, #0xFFFFC03F // Mask MODERs
	orrs r2, r2, r0
	str r2, [r1]

	// Get output register address
	ldr r3, =GPIOB_ODR

	bx lr

DisplayLED: // r1 = shift state, r3 = GPIOB_ODR
	mvn r5, r1
	lsl r5, #3
	str r5, [r3]

	bx lr

Delay:
	ldr r5, =X
L1:
	ldr r6, =Y //+2
	subs r5, r5, #1 //+1
	bne L2 //1

	bx lr
L2:
	subs r6, r6, #1 //+1
	bne L2 //+1
	b L1

*/
