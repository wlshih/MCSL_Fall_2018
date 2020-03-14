	.syntax unified
	.cpu cortex-m4
	.thumb
.data
	.equ RCC_AHB2ENR, 0x4002104C
	.equ GPIOB_MODER, 0x48000400
	.equ GPIOB_OTYPER, 0x48000404
	.equ GPIOB_OSPEEDR, 0x48000408
	.equ GPIOB_PUPDR, 0x4800040C
	.equ GPIOB_ODR, 0x48000414
	.equ GPIOB_BSRR, 0x48000418
	.equ GPIOB_BRR, 0x48000428
	.equ GPIOC_MODER, 0x48000800
	.equ GPIOC_IDR, 0x48000810

	.equ X, 1000
	.equ Y, 500
	.equ tenThousand, 10000
	.equ hundredThousand, 100000
	.equ million, 1000000
	.equ tenMillion, 10000000
	.equ hundredMillion, 100000000
.text
	.global main
main:
	BL GPIO_init
	BL max7219_init
	mov r4,#0
	mov r3,#1
	mov r2,#0
	mov r12,#0
	b fib
//
fib:
	//r0,r1不能用
	//r3一開始等於1, r2 = 0
	//r4 = r3+r2
	//一輪之後, r3 = r4, r2 = r3
	push {r4}
	//one-digit
	add r12,r12,#1

	cmp r12,#8
	it lt
	blt one_digit
	//two-digit
	cmp r12,#13
	it lt
	blt two_digit
	//three-digit
	cmp r12,#18
	it lt
	blt three_digit
	//four-digit
	cmp r12, #22
	it lt
	blt four_digit
	//five-digit
	cmp r12, #27
	it lt
	blt five_digit
	//six-digit
	cmp r12, #32
	it lt
	blt six_digit
	//seven-digit
	cmp r12, #37
	it lt
	blt seven_digit
	//eight-digit
	cmp r12, #41
	it lt
	blt eight_digit

	b out_of_bound
program_end:
	b program_end
out_of_bound:
	//scan-limit setting
	movs r0,#0xB//scan-limit address
	movs r1,#1//display digit 0,1
	bl MAX7219Send
	//decode mode setting
	movs r0,#0x9//decode address
	movs r1,#0x0//no decode
	bl MAX7219Send
	//send '-' to digit1
	movs r0,#2
	movs r1,#1
	bl MAX7219Send
	//send '1' to digit0
	movs r0,#1
	movs r1,#0x30
	bl MAX7219Send
	//
	b program_end
one_digit:
	//scan-limit setting
	movs r0,#0xB//scan-limit address
	movs r1,#0//display digit 0 only
	bl MAX7219Send
	//show number
	movs r0,#1
	movs r1,r4
	bl MAX7219Send
	//
	b cal_next_num
two_digit:
	//scan-limit setting
	movs r0,#0xB//scan-limit address
	movs r1,#1//display digit 0,1
	bl MAX7219Send
	//show number
two_2:
	mov r10,#10
	sdiv r2,r4,r10 //r2 = r4/10 (eg.r4 = 23,r2 = 2)
	movs r0,#2
	movs r1,r2
	bl MAX7219Send
	mov r10,#10
	mul r2,r2,r10  //r2 = 20，r4-r2就可以得到最低位元
	sub r2,r4,r2 //r2 = r4-r2 = 最低位元
	movs r0,#1
	movs r1,r2
	bl MAX7219Send
	//
	b cal_next_num
three_digit:
	//scan-limit setting
	movs r0,#0xB//scan-limit address
	movs r1,#2//display digit 0,1,2
	bl MAX7219Send
three_2:
	//show 百分位
	mov r10,#100
	sdiv r2,r4,r10 //r2 = r4/100
	movs r0,#3
	movs r1,r2
	bl MAX7219Send
	//r4 = r4去掉百分位
	mov r10,#100
	mul r2,r2,r10
	sub r4,r4,r2
	//goto two_2
	b two_2

four_digit:
	//scan-limit setting
	movs r0,#0xB//scan-limit address
	movs r1,#3//display digit 0,1,2,3
	bl MAX7219Send
four_2:
	//show 千分位
	mov r10,#1000
	sdiv r2,r4,r10 //r2 = r4/1000
	movs r0,#4
	movs r1,r2
	bl MAX7219Send
	//r4 = r4去千分位
	mov r10,#1000
	mul r2,r2,r10
	sub r4,r4,r2
	//goto three_2
	b three_2
five_digit:
	//scan-limit setting
	movs r0,#0xB//scan-limit address
	movs r1,#4//display digit 0,1,2,3,4
	bl MAX7219Send
five_2:
	//show 萬分位
	ldr r10,=tenThousand
	sdiv r2,r4,r10 //r2 = r4/10000
	movs r0,#5
	movs r1,r2
	bl MAX7219Send
	//r4 = r4去萬分位
	ldr r10,=tenThousand
	mul r2,r2,r10
	sub r4,r4,r2
	//goto four_2
	b four_2
six_digit:
	//scan-limit setting
	movs r0,#0xB//scan-limit address
	movs r1,#5//display digit 0,1,2,3,4,5
	bl MAX7219Send
six_2:
	//show 萬分位
	ldr r10,=hundredThousand
	sdiv r2,r4,r10 //r2 = r4/100000
	movs r0,#6
	movs r1,r2
	bl MAX7219Send
	//r4 = r4去十萬分位
	ldr r10,=hundredThousand
	mul r2,r2,r10
	sub r4,r4,r2
	//goto five_2
	b five_2
seven_digit:
	//scan-limit setting
	movs r0,#0xB//scan-limit address
	movs r1,#6//display digit 0,1,2,3,4,5,6
	bl MAX7219Send
seven_2:
	//show
	ldr r10,=million
	sdiv r2,r4,r10 //r2 = r4/1000000
	movs r0,#7
	movs r1,r2
	bl MAX7219Send
	//
	ldr r10,=million
	mul r2,r2,r10
	sub r4,r4,r2
	//
	b six_2
eight_digit:
	//scan-limit setting
	movs r0,#0xB//scan-limit address
	movs r1,#7//display digit 0,1,2,3,4,5,6,7
	bl MAX7219Send
eight_2:
	//show
	ldr r10,=tenMillion
	sdiv r2,r4,r10 //r2 = r4/10000000
	movs r0,#8
	movs r1,r2
	bl MAX7219Send
	//
	ldr r10,=tenMillion
	mul r2,r2,r10
	sub r4,r4,r2
	//
	b seven_2
cal_next_num:
	//wait 0.5s
	bl Delay
	//
	pop {r4}
	//calculate next number
	mov r2,r3
	mov r3,r4
	add r4,r2,r3
	//wait for button_push
	mov r5,#0//initialize counter in wait_button
	bl wait_button
	//
	b fib
wait_button:
	//r0,r1,r2,r5,ok
	//r3,r4 do not use
	ldr r0,=GPIOC_IDR
	ldr r1,[r0]
	and r1,r1,0x2000
	lsr r1,#13
	ldr r2,[r0]
	and r2,0x2000
	lsr r2,#13
	and r1,r1,r2
	//if r1 and r2 == 0,cnt++
	cmp r1,#0
	it eq
	addeq r5,r5,#1
	//if r1 and r2 ==1, cnt = 0
	cmp r1,#1
	it eq
	moveq r5,#0
	//if cnt == 20, button has been pushed
	cmp r5,#20
	itt eq
	moveq r5,#0
	bxeq lr
	//
	b wait_button


GPIO_init:
//TODO: Initialize three GPIO pins as output for max7219 DIN, CS and CLK

	// Enable AHB2 GPIOB&C clock
	movs r0, #0x6
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

	// Set PC13 as input mode
	ldr 	r1, =GPIOC_MODER
	ldr 	r0, [r1]
	ldr 	r2, =0xF3FFFFFF //pc13
	and 	r0, r2
	str		r0, [r1]
	//
	ldr 	r11, =GPIOC_IDR

	bx lr

MAX7219Send:

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
