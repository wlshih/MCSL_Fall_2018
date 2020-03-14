	.syntax unified
	.cpu cortex-m4
	.thumb
.data
	arr: .byte 0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0x77, 0x1F, 0x4E, 0x3D, 0x4F, 0x47 //TODO: put 0 to F 7-Seg LED pattern here
	.equ RCC_AHB2ENR, 0x4002104C
	.equ GPIOB_MODER, 0x48000400
	.equ GPIOB_OTYPER, 0x48000404
	.equ GPIOB_OSPEEDR, 0x48000408
	.equ GPIOB_PUPDR, 0x4800040C
	.equ GPIOB_ODR, 0x48000414
	.equ GPIOB_BSRR, 0x48000418
	.equ GPIOB_BRR, 0x48000428

	.equ X, 1000
	.equ Y, 1500
.text
	.global main
main:
	BL GPIO_init
	BL max7219_init
	mov r0,#1
	mov r1,#0
loop:
	BL Display0toF
	//decode
	movs r0,#0x9//decode address
	movs r1,#1//decode digit 0 only
	bl MAX7219Send
	B loop
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
Display0toF:
//TODO: Display 0 to F at first digit on 7-SEG LED. Display one per second.
	//use r4 for cnt
	movs r4,#0//initial r4 to 0
	//save link register
	push {lr}

	//load arr, r2 for address
	ldr r2, =arr
display_inner_loop:
	//r3 for arr value
	ldrb r3, [r2]
	//max7219send
	movs r0,#0x1//digit 0 address(D11~D8)
	movs r1,r3//0~F
	bl MAX7219Send
	bl Delay
	//將r2指到陣列的下一個位置
	add r2,r2,#1//加一個byte就好
	add r4,r4,#1//cnt++
	//if cnt == 10, change decode mode to no decode mode
	cmp r4,#10
	ittt eq
	moveq r0,#0x9//decode address
	moveq r1,#0//no decode
	bleq MAX7219Send
	//if cnt < 16, keep doing display, else end display0tof
	cmp r4,#16
	it lt
	blt display_inner_loop

	pop {pc}
MAX7219Send:
//注意:r2,r3,r4不能動到
//input parameter: r0 is ADDRESS(4-bit,d11~d8) , r1 is DATA(8-bit,d7~d0)
//TODO: Use this function to send a message to max7219

	//make r5  be the serial number which is used to send into max7219
	mov r5,r0
	lsl r5,#8
	add r5,r1


	//odr: odr[5]: clk, odr[4]: cs, odr[3]: din
    //一定要先將din值設定好再將clk值設成1，不然讀不到
	movs r6,#0//r6 for count send_inner_loop，先初始化
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
	movs r1,#1//decode digit 0 only
	bl MAX7219Send

	//intensity setting
	movs r0,#0xA//intensity address
	movs r1,#0x5//最亮
	bl MAX7219Send

	//scan-limit setting
	movs r0,#0xB//scan-limit address
	movs r1,#0//display digit 0 only
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

