/*	.syntax unified
	.cpu cortex-m4
	.thumb

.data
	leds: .byte 0
	dir: .word 0
	button: .word 0
	button_prev: .word 1
	pause: .word 0

.text
	.global main
	.equ RCC_AHB2ENR, 0x4002104C

	.equ GPIOB_MODER, 0x48000400
	.equ GPIOB_OTYPER, 0x48000404
	.equ GPIOB_OSPEEDR, 0x48000408
	.equ GPIOB_PUPDR, 0x4800040C
	.equ GPIOB_ODR, 0x48000414

	.equ GPIOC_MODER, 0x48000800
	.equ GPIOC_PUPDR, 0x4800080C
	.equ GPIOC_IDR, 0x48000810

	.equ X, 326 // poll count
	.equ Y, 2400 // poll delay
	.equ Z, 20 // debounce


main: // r0 = leds address, r1, r2 = state
	bl GPIO_init

	movs r1, #1
	movs r2, #3
	ldr r0, =leds
	strb r1, [r0]

Loop: // r4 = flag
	bl DisplayLED
	bl Delay

	ldr r5, =dir
	ldr r4, [r5]
	cmp r1, #8 // Set the direction flag
	it eq
	moveq r4, #0
	cmp r1, #1
	it eq
	moveq r4, #1
	str r4, [r5] // Save direction flag

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
	movs r0, #0x6
	ldr r1, =RCC_AHB2ENR
	str r0, [r1]

	// Set PB3~6 as output mode
	movs r0, #(0x55<<6) // 0x01
	ldr r1, =GPIOB_MODER
	ldr r2, [r1]
	and r2, #0xFFFFC03F // Mask MODERs
	orrs r2, r2, r0
	str r2, [r1]
	// Set PC13 as input mode
	movs r0, #0
	ldr r1, =GPIOC_MODER
	ldr r2, [r1]
	and r2, #0xF3FFFFFF // Mask MODER
	orrs r2, r2, r0
	str r2, [r1]

	// Set PB3~6 as push-pull output
	movs r0, #(0x0<<3) // 0x0
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
	// Get button register address
	ldr r7, =GPIOC_IDR

	bx lr

DisplayLED: // r1 = shift state, r3 = GPIOB_ODR
	mvn r5, r1
	lsl r5, #3
	str r5, [r3]

	bx lr

Delay:
	ldr r4, =X
	push {lr}
L1:
	bl Button_polling /////////////////////////////////////////
	ldr r5, =pause    //                                     //
	ldr r6, [r5]      // Delete this block to disable button //
	cmp r6, #0        //                                     //
	bne L1            /////////////////////////////////////////

	sub r4, r4, #1
	cmp r4, #0
	beq Break
	ldr r5, =Y
L2:
	sub r5, r5, #1
	cmp r5, #0
	bne L2
	b L1
Break:
	pop {pc}

Button_polling: // r7 = GPIOC_IDR address, r8 = input wave value
	ldr r6, =Z // Set debounce counter
	ldr r8, [r7]
Debounce: // r7 = GPIOC_IDR address, r6 = debounce cntr
	mov r9, r8
	ldr r8, [r7]
	cmp r9, r8
	ite eq
	subeq r6, #1
	ldrne r6, =Z

	cmp r6, #0
	bne Debounce
	lsr r8, #13
Switch_handle: // r8 = GPIOC_IDR value
	ldr r10, =button // Update button & button_prev
	ldr r9, [r10]
	str r8, [r10]
	ldr r10, =button_prev
	str r9, [r10]
	// r8 = button value, r9 = button_prev value
	cmp r8, r9 // Go to pause_handle when negative-edge, else continue to finish Delay
	blt Pause_handle
	bx lr
Pause_handle:
	ldr r8, =pause
	ldr r9, [r8]
	eor r9, r9, 0x00000001 // Update pause
	str r9, [r8]
	cmp r9, #0 // Continue to finish the Delay loop when pause = 0
	bne Button_polling

	bx lr

*/
