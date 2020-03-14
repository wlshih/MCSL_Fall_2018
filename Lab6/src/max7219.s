	.syntax unified
	.cpu cortex-m4
	.thumb
.data
	student_id: .byte 0, 5, 1, 6, 0, 7, 6 //TODO: put your student id here

.text
	//.global main
	.global GPIO_init
	.global max7219_init
	.global max7219_send

	.equ RCC_AHB2ENR, 0x4002104C
	.equ GPIOA_BASE, 0x48000000
	.equ GPIOA_MODER, 0x48000000
	.equ GPIOA_OTYPER, 0x48000004
	.equ GPIOA_OSPEEDR, 0x48000008
	.equ GPIOA_PUPDR, 0x4800000C
	.equ GPIOA_ODR, 0x48000014
	.equ GPIOA_BSRR_OFFSET, 0x18
	.equ GPIOA_BRR_OFFSET, 0x28

	.equ DECODE_MODE, 0x9
	.equ INTENSITY, 0xA
	.equ SCAN_LIMIT, 0xB
	.equ SHUTDOWN, 0xC
	.equ DISPLAY_TEST, 0xF
	.equ LOAD, 0x40
	.equ DATA, 0x20
	.equ CLK, 0x80

main:
	bl GPIO_init
	bl max7219_init
	bl show_id
Program_end:
	b Program_end

show_id:
	push {r0, r1, r2, r3, lr}
	ldr r2, =student_id
	movs r0, #7 // r0 = address(4-bit)
show_id_loop:
	ldrb r3, [r2] // value in student_id
	movs r1, r3 // r1 = data(8-bit)
	bl max7219_send
	add r2, r2, #1
	sub r0, r0, #1
	cmp r0, #0
	bne show_id_loop

	pop {r0, r1, r2, r3, pc}

GPIO_init:
//TODO: Initialize three GPIO pins as output for max7219 DIN, CS and CLK
	push {r0, r1, r2, lr}
	// Enable AHB2 GPIOA clock
	movs r0, #0x1
	ldr r1, =RCC_AHB2ENR
	str r0, [r1]

	// Set PA5~7 as output mode
	movs r0, #0x5400 // 0x010101, set pa5~7 as output
	ldr r1, =GPIOA_MODER
	ldr r2, [r1]
	and r2, #0xFFFF03FF // Mask MODERs
	orrs r2, r2, r0
	str r2, [r1]


	// Set PA5~7 as high speed mode
	movs r0, #(0x2A<<10) // set them as 0x101010
	ldr r1, =GPIOA_OSPEEDR
	ldr r2, [r1]
	and r2, #0xFFFF03FF // Mask SPEEDRs
	orrs r2, r2, r0
	str r2, [r1]

	pop {r0, r1, r2, pc}

max7219_init:
// Initialize max7219 registers

	push {r0, r1, lr}//save link register

	//decode mode setting
	ldr r0, =DECODE_MODE // 0xX9
	movs r1, #0xFF // decode all digits
	bl max7219_send

	//intensity setting
	ldr r0, =INTENSITY // 0xXA
	movs r1, #0xA // from 0~F
	bl max7219_send

	//scan-limit setting
	ldr r0, =SCAN_LIMIT // 0xXB
	movs r1, #0 // display digit 0 only
	bl max7219_send

	//shutdown setting(normal operation)
	ldr r0, =SHUTDOWN // 0xXC
	movs r1, #1 // normal operation
	bl max7219_send

	//display-test setting
	ldr r0, =DISPLAY_TEST // 0xF
	//movs r1,#1 // display-test
	movs r1, #0 // not display-test, just normal operation
	bl max7219_send

	//display initial 0
	mov r0, #1
	mov r1, #0
	bl max7219_send

	pop {r0, r1, pc}

max7219_send:
   //input parameter: r0 is ADDRESS , r1 is DATA
	//TODO: Use this function to send a message to max7219
	push {r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, lr}
	lsl r0, r0, #8
	add r0, r0, r1
	ldr r1, =#GPIOA_BASE
	ldr r2, =LOAD // 0x40
	ldr r3, =DATA // 0x20
	ldr r4, =CLK // 0x80
	ldr r5, =GPIOA_BSRR_OFFSET // 0x18
	ldr r6, =GPIOA_BRR_OFFSET // 0x28
	mov r7, #16 // r7 = i
.max7219send_loop:
	mov r8, #1
	sub r9, r7, #1
	lsl r8, r8, r9 // r8 = mask
	str r4, [r1,r6] //HAL_GPIO_WritePin(GPIOA, CLOCK, 0);
	tst r0, r8
	beq .bit_not_set //bit not set
	str r3, [r1,r5]
	b .if_done
.bit_not_set:
	str r3, [r1,r6]
.if_done:
	str r4, [r1,r5]
	subs r7, r7, #1
	bgt .max7219send_loop
	str r2, [r1,r6]
	str r2, [r1,r5]
	pop {r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, pc}
	BX LR
