	.syntax unified
	.cpu cortex-m4
	.thumb
.data
	arr: .space 400

.text
	.global main
	.equ N, 47

fib:
	cmp r3, #100
	bgt exit
	cmp r3, #1
	blt exit

	add r0, r0, #4
	adds r4, r1, r2

	itt vs
	movvs r4, #0
	subvs r4, r4, #2

	str r4, [r0]

	mov r1, r2
	mov r2, r4
	
	sub r3, #1
	cmp r3, #0
	bne fib

	bx lr

exit:
	mov r4, #0
	sub r4, r4, #1

	bx lr

main:
	ldr r0, =arr
	mov r1, #1
	str r1, [r0]
	add r0, r0, #4
	mov r2, #1
	str r2, [r0]

	movs r3, #N
	sub r3, #2
	bl fib

L: b L
