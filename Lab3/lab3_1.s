	.syntax unified
	.cpu cortex-m4
	.thumb
.data
	user_stack: .zero 128
	array_start: .word 0x20018000 //address decrease
	//new_stack_start: .word 0x20019000 //address decrease
	expr_result: .word 0
.text
	.global main
	postfix_expr: .asciz "5 4 -"

main:
	LDR r0, =postfix_expr
	movs r3,#10 //initilize r3 to 10
	movs r4,#0 //represent variable negative
	movs r5,#0 //represent temp int
	movs r6,#0 //constant zero
	movs r7,#0 //represent if the number is empty,0 means empty , 1 means not empty

	BL atoi //string to integer



	ldr r0,=array_start//r0 store the array pointer
	ldr r1,[r0]// r1 store the  value
	//ldr r1,=new_stack_start
	//msr msp, r1 //change main stack pointer to a new place
	B calculate_expr_result

	//TODO: Setup stack pointer to end of user_stack and calculate the
	//expression using PUSH, POP operators, and store the result into
	//expr_result
program_end:
	B program_end
atoi:
	//IT{x{y{z}}} {cond}
	//其中xyz只可以用T或者E，T就是Then，条件为真时执行，E是Else，条件为假时执行
	//TODO: implement a “convert string to integer” function

	ldrb r1,[r0]//load lowest 8 bit(1 byte)of values in r0 into r1
	add r0,r0,#1//to get next char
	ldrb r2,[r0]//load next char
	sub r0,r0,#1//get back to right now string pointer

	//check_minus_or_negative
	cmp r1, #45
	beq check_minus_or_negative

	//IT block for  pushing "+" into stack
	cmp r1, #43
	ITTT eq
	pusheq {r1}
	addeq r0,r0,#1
	beq atoi

	//IT block for  pushing "minus symbol-" into stack
	/*
	adds r1,r1,r4//use variable negative to check if it is minus
	cmp r1, #45
	ITTE eq
	ldrbeq r1,[r0] //get correct r1 value back
	pusheq {r1}
	ldrbne r1,[r0]
	*/


	//branch to push_int_space to push integer into stack
	cmp r1, #32
	beq push_int_space

	//branch to push_int_end to push integer into stack & end for loop
	cmp r1, #0
	beq push_int_end

	//
	cmp r1,#47 //'0'~'9' = 48~57
	ITTTT gt
	subgt r1,r1,#48 //make char num to int num
	mulgt r5,r5,r3
	addgt r5,r5,r1
	movgt r7,#1

	//for loop doing i++
	add r0,r0,#1 //go to next char's address
	b atoi //if r1 != '\0',then keep doing atoi

	//BX LR

push_int_space:
	cmp r4, #1 //test if temp is a negative number
	IT eq
	subeq r5,r6,r5

	cmp r7, #1 //test if temp is empty or not
	ITTTT eq
	pusheq {r5}	//push temp into stack
	moveq r4, #0 //negative = false
	moveq r5, #0 //temp = 0
	moveq r7, #0 //empty = 0

	add r0,r0,#1
	b atoi
push_int_end:
	cmp r4, #1 //test if temp is a negative number
	IT eq
	subeq r5,r6,r5

	cmp r7, #1 //test if temp is empty or not
	ITTTT eq
	pusheq {r5}	//push temp into stack
	moveq r4, #0 //negative = false
	moveq r5, #0 //temp = 0
	moveq r7, #0 //empty = 0

	//add r0,r0,#1
	push {r6}
	bx lr
check_minus_or_negative:
	cmp r2, #32
	ITE ne
	movne r4,#1
	pusheq {r1}

	cmp r2, #0
	IT eq
	pusheq {r1}

	add r0,r0,#1
	b atoi
calculate_expr_result:
	//use r4, r5,r6 to do stackt calculation,r4~r5 is used to store pop value, r6 is the answer
	sub r1,r1,#4
	ldr r2,[r1] //load the value in r0 to r1

	cmp r2,#43
	ITTTT eq
	popeq {r4,r5}
	addeq r6, r4,r5
	pusheq {r6}
	beq calculate_expr_result

	cmp r2,#45
	ITTTT eq
	popeq {r4,r5}
	subeq r6, r5,r4
	pusheq {r6}
	beq calculate_expr_result

	cmp r2,#0
	ITTTT eq
	popeq {r6}
	ldreq r0,=expr_result
	streq r6,[r0]
	beq program_end

	push {r2}
	b calculate_expr_result


