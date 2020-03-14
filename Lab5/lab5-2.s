	.syntax unified
	.cpu cortex-m4
	.thumb
.data
	student_id: .byte 9, 3, 0, 6, 1, 5, 0 //TODO: put your student id here
	.equ RCC_AHB2ENR, 0x4002104C
	.equ GPIOB_MODER, 0x48000400
	.equ GPIOB_OTYPER, 0x48000404
	.equ GPIOB_OSPEEDR, 0x48000408
	.equ GPIOB_PUPDR, 0x4800040C
	.equ GPIOB_ODR, 0x48000414
	.equ GPIOB_BSRR, 0x48000418
	.equ GPIOB_BRR, 0x48000428
.text
	.global main
main:
	BL GPIO_init
	BL max7219_init
	//TODO: display your student id on 7-Seg LED
	bl show_id
Program_end:
	B Program_end
GPIO_init:
//TODO: Initialize three GPIO pins as output for max7219 DIN, CS and CLK

	// Enable AHB2 GPIOB clock
	movs r0, #0x2
	ldr r1, =RCC_AHB2ENR
	str r0, [r1]

	// Set PB3~5 as output mode
	movs r0, #(0x15<<6) // 0x010101000000, set pb3~5 as output
	ldr r1, =GPIOB_MODER
	ldr r2, [r1]
	and r2, #0xFFFFF03F // Mask MODERs, 1^20, 000000, 1^6
	orrs r2, r2, r0
	str r2, [r1]

	// Set PB3~5 asOD output
	movs r0, #(0x0<<3) // set them as 0x0
	ldr r1, =GPIOB_OTYPER
	ldr r2, [r1]
	and r2, #0xFFFFFFC7 // Mask MODERs
	orrs r2, r2, r0
	str r2, [r1]

	// Set PB3~5 as high speed mode
	movs r0, #(0x2A<<6) // set them as 0x10
	ldr r1, =GPIOB_OSPEEDR
	ldr r2, [r1]
	and r2, #0xFFFFF03F // Mask MODERs
	orrs r2, r2, r0
	str r2, [r1]

	// Set PB3~5 as pull-down
	movs r0, #(0x2A<<6) // 0x10
	ldr r1, = GPIOB_PUPDR
	ldr r2, [r1]
	and r2, #0xFFFFF03F // Mask MODERs
	orrs r2, r2, r0
	str r2, [r1]

	bx lr
MAX7219Send:
//注意:r2,r3,r4不能動到
//input parameter: r0 is ADDRESS(4-bit,d11~d8) , r1 is DATA(8-bit,d7~d0)
//TODO: Use this function to send a message to max7219

	//make r5  be the serial number which is used to send into max7219
	mov r5,r0
	lsl r5,#8
	add r5,r1

	//r6 for cnt send_inner_loop,r7 是odr's address, r8 是用來存進odr的值
	//odr: odr[5]: clk, odr[4]: cs, odr[3]: din
	movs r6,#0//initialize
	//先送4次零來補d15~d12
	ldr r10,=GPIOB_BSRR
	ldr r11,=GPIOB_BRR
	ldr r7, =GPIOB_ODR

	//din=0
	mov r8,#(1<<3)
	str r8,[r11]
	//clk = 1
	mov r8,#(1<<5)
	str r8,[r10]
	//clk = 0
	mov r8,#(1<<5)
	str r8,[r11]
	//clk = 1
	mov r8,#(1<<5)
	str r8,[r10]
	//clk = 0
	mov r8,#(1<<5)
	str r8,[r11]
	//clk = 1
	mov r8,#(1<<5)
	str r8,[r10]
	//clk = 0
	mov r8,#(1<<5)
	str r8,[r11]
	//clk = 1
	mov r8,#(1<<5)
	str r8,[r10]
	//clk = 0
	mov r8,#(1<<5)
	str r8,[r11]
	//clk = 1
	mov r8,#(1<<5)
	str r8,[r10]
	//clk = 0
	mov r8,#(1<<5)
	str r8,[r11]
	//r9 is shift cnt
	mov r9,#11

send_inner_loop:
//r6 for cnt, r8 用來存r5 bitAnd 1 完的結果並放進odr!, r9 is shift cnt
//r10 = bsrr,r11 = brr(bsrr low 16-bit 如果是1的話就改1，brr如果是1的話改0)
	push {lr}
	mov r8,r5
	lsr r8,r9
	and r8,#1

	//if r8==1, 先讓din改變,再讓clk=1,最後變成odr = 10100, else odr = 10000
	cmp r8,#1
	it eq
	bleq din_1
	cmp r8,#0
	it eq
	bleq din_0

	//cnt++
	adds r6,#1
	//if cnt < 12, keep doing send_inner_loop, else cs = 1,to letch the d15~d0
	cmp r6,#12
	itt lt
	sublt r9,#1
	blt send_inner_loop
	//set cs = 1,to letch the d15~d0
	mov r8,#(1<<4)
	str r8,[r10]
	//reset cs = 0
	mov r8,#(1<<4)
	str r8,[r11]
	//
	pop {pc}
max7219_init:
//TODO: Initialize max7219 registers

	push {lr}//save link register

	//decode mode setting
	movs r0,#0x9//decode address
	movs r1,#0xFF//decode all digits
	bl MAX7219Send

	//intensity setting
	movs r0,#0xA//intensity address
	movs r1,#0x5//中等亮
	bl MAX7219Send

	//scan-limit setting
	movs r0,#0xB//scan-limit address
	movs r1,#6//display digit 0~6 only
	bl MAX7219Send

	//shutdown setting(normal operation)
	movs r0,#0xC//shutdown address
	movs r1,#1//normal operation
	bl MAX7219Send

	//display-test setting
	movs r0,#0xF
	//movs r1,#1//display-test
	movs r1,#0//not display-test, just normal operation
	bl MAX7219Send

	pop {pc}
show_id:
	push {lr}
	ldr r2,=student_id
	ldrb r3,[r2]//value in student_id
	movs r0,#1
	movs r1,r3
	bl MAX7219Send
	//
	add r2,r2,#1
	ldrb r3,[r2]
	movs r0,#2
	movs r1,r3
	bl MAX7219Send
	//
	add r2,r2,#1
	ldrb r3,[r2]
	movs r0,#3
	movs r1,r3
	bl MAX7219Send
	//
	add r2,r2,#1
	ldrb r3,[r2]
	movs r0,#4
	movs r1,r3
	bl MAX7219Send
	//
	add r2,r2,#1
	ldrb r3,[r2]
	movs r0,#5
	movs r1,r3
	bl MAX7219Send
	//
	add r2,r2,#1
	ldrb r3,[r2]
	movs r0,#6
	movs r1,r3
	bl MAX7219Send
	//
	add r2,r2,#1
	ldrb r3,[r2]
	movs r0,#7
	movs r1,r3
	bl MAX7219Send
	//
	pop {pc}
	//
din_1:
	mov r0,#(1<<3)//din=1
	str r0,[r10]
	mov r0,#(1<<5)//clk=1
	str r0,[r10]
	mov r0,#(1<<5)//clk=0
	str r0,[r11]
	bx lr
din_0:
	mov r0,#(1<<3)//din=0
	str r0,[r11]
	mov r0,#(1<<5)//clk=1
	str r0,[r10]
	mov r0,#(1<<5)//clk=0
	str r0,[r11]
	bx lr
