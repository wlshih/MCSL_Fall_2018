	.syntax unified
	.cpu cortex-m4
	.thumb
.data
	arr1: .byte 0x19, 0x34, 0x14, 0x32, 0x52, 0x23, 0x61, 0x29
	arr2: .byte 0x18, 0x17, 0x33, 0x16, 0xFA, 0x20, 0x55, 0xAC

.text
	.global main

do_sort:
	mov r1, #8

	b outer_loop

outer_loop:
	sub r1, r1, #1
	cmp r1, #0
	ittt ne
	movne r6, r0
	movne r2, r1

	bne inner_loop

	bx lr

inner_loop:

	cmp r2, #0
	beq outer_loop

	ldrb r3, [r6]
	ldrb r4, [r6, #1]
	cmp r3, r4
	ittt gt
	movgt r5, r3
	movgt r3, r4
	movgt r4, r5
	strb r3, [r6]
	add r6, r6, #1
	strb r4, [r6]

	sub r2, r2, #1

	b inner_loop

main:
	ldr r0, =arr1
	bl do_sort
	ldr r0, =arr2
	bl do_sort

L: b L
