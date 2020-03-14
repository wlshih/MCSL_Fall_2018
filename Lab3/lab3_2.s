	.syntax unified
	.cpu cortex-m4
	.thumb
.data
	result: .word 0
	max_size: .word 0
.text
	.global main
	m: .word 8
	n: .word 2
main:
	ldr r0,=m
	ldr r1,=n
	ldr r2,[r0]//value of m
	ldr r3,[r1]//value of n
	//push {r2,r3}
	movs r0,r2
	movs r1,r3
	push {r0,r1}
	movs r5, #0 //r5 represent the cnt of *2
	movs r6, #2 //constant 2
	movs r7, #0 //max_size
	movs r9, #32 //constant 32
	bl GCD
	b get_answer
GCD:
	//TODO: Implement your GCD function
	pop {r0,r1} //get a,b
	mov r4,#0 //initilize r4, which is an odd-even indicator
	//if(a<b) exchange a b
	cmp r0,r1
	ittt lt
	movlt r2,r0
	movlt r0,r1
	movlt r1,r2



    /*****************test a,b are odd or even numbers************************/
	//test a,if r2 = 1, means a is an odd number,
	//otherwise it is an even number
	mov r2,r0
	lsl r2,#31
	lsr r2,#31
	//test b,if r3 = 1, means b is an odd number,
	//otherwise it is an even number
	mov r3,r1
	lsl r3,#31
	lsr r3,#31
	//if a is odd, add 10 to r4. if b is odd, add 1 to r4
	cmp r2, #1
	it eq
	addeq r4, #10

	cmp r3, #1
	it eq
	addeq r4, #1

	//if((!(a&1))&&(!(b&1))) return stein(a>>1,b>>1)<<1;
	cmp r4, #0
	ITTT eq
	lsreq r0,#1
	lsreq r1,#1
	addeq r5,r5,#1


	//if((a&1)&&(!(b&1)))return stein(a,b>>1);
	cmp r4, #10
	it eq
	lsreq r1,#1

	//if((!(a&1))&&(b&1))return stein(a>>1,b);
	cmp r4, #1
	it eq
	lsreq r0,#1

	//else return stein(a-b,b);
	cmp r4, #11
	it eq
	subeq r0,r0,r1


	//b GCD


	//if(b==0)return a;
	cmp r1,#0
	ittt ne
	pushne {r0,r1,lr}
	addne r7,r7,#1
	blne GCD


	pop {pc}
	BX LR


program_end:
	B program_end
	//r0 stores the answer

get_answer:
	cmp r5,#0
	beq get_max_size

	mul r0,r0,r6
	sub r5,r5,#1
	b get_answer

get_max_size:
	add r7,r7,#3
	mul r7,r7,r9
	ldr r8,=max_size
	str r7,[r8]
	b program_end



