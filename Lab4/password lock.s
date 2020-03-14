	.syntax unified
	.cpu cortex-m4
	.thumb

.data
	password: .byte 0x7
	button: .word 0
	button_prev: .word 1

.text
	.global main
	.equ RCC_AHB2ENR, 0x4002104C

	.equ GPIOA_MODER, 0x48000000
	.equ GPIOA_PUPDR, 0x4800000C
	.equ GPIOA_IDR, 0x48000010

	.equ GPIOB_MODER, 0x48000400
	.equ GPIOB_OTYPER, 0x48000404
	.equ GPIOB_OSPEEDR, 0x48000408
	.equ GPIOB_PUPDR, 0x4800040C
	.equ GPIOB_ODR, 0x48000414

	.equ GPIOC_MODER, 0x48000800
	.equ GPIOC_IDR, 0x48000810

	.equ X, 326 // delay
	.equ Y, 730 // delay
	.equ Z, 20 // debounce count


main:
	bl GPIO_init

Loop: // r0 = DIP_i address, r1 = LED_o address, r2 = button_i address
	bl Button_polling
	bl ReadDIP

	b Loop


GPIO_init:
	// Enable AHB2 GPIOA~C clock
	movs r0, #0x7
	ldr r1, =RCC_AHB2ENR
	str r0, [r1]

	// Set PA5~8 as input mode
	movs r0, #0
	ldr r1, =GPIOA_MODER
	ldr r2, [r1]
	and r2, #0xFFFC03FF // Mask MODER
	orrs r2, r2, r0
	str r2, [r1]
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

	// Get DIP input address
	ldr r0, =GPIOA_IDR
	// Get LED output address
	ldr r1, =GPIOB_ODR
	// Get button input address
	ldr r2, =GPIOC_IDR

	// Reset LED output register
	ldr r3, [r1]
	and r3, 0xFFFFFF87 // Mask MODERs
	orr r3, 0x00000078
	str r3, [r1]

	bx lr

Button_polling: // r2 = button_i address, r4 = input wave value
	ldr r3, =Z // Set debounce counter
	ldr r4, [r2]
Debounce:
	mov r5, r4
	ldr r4, [r2]
	cmp r5, r4
	ite eq
	subeq r3, #1
	ldrne r3, =Z

	cmp r3, #0
	bne Debounce
	lsr r4, #13 // r4 = button input value
Switch_handle:
	ldr r3, =button // Update button & button_prev
	ldr r5, [r3]
	str r4, [r3]
	ldr r3, =button_prev
	str r5, [r3]
	// r4 = button value, r5 = button_prev value
	cmp r4, r5 // Break the loop when negative-edge, else continue to poll
	bge Button_polling
	bx lr

ReadDIP: // r0 = DIP_i address
	ldr r3, =password
	ldrb r4, [r3]
	ldr r3, [r0]
	orr r3, 0xFFFFFE1F // Get clear value of DIP input
	mvn r3, r3 // Input value inversion
	lsr r3, #5
	cmp r3, r4
	bne Access_denied
Access_confirmed:
	push {lr}
	bl BlinkLED
	bl BlinkLED
	bl BlinkLED
	pop {pc}
Access_denied:
	push {lr}
	bl BlinkLED
	pop {pc}

BlinkLED: // r1 = LED_o address
	push {lr}
	bl Delay // Open light
	ldr r3, [r1]
	and r3, 0xFFFFFF87
	str r3, [r1]
	bl Delay // Close light
	orr r3, 0x00000078
	str r3, [r1]

	pop {pc}
Delay:
	ldr r3, =X
L1:
	sub r3, r3, #1
	cmp r3, #0
	beq Break
	ldr r4, =Y
L2:
	sub r4, r4, #1
	cmp r4, #0
	bne L2
	b L1
Break:
	bx lr


