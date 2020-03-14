.data
	result: .byte 0

.text
	.global main
	.equ X, 0x55AA
	.equ Y, 0xAA55

hamm:
	eor r0, r1
	lsr r1, r0, #1
	ldr r2, =0x5555
	and r0, r2
	and r1, r2

	add r0, r0, r1
	lsr r1, r0, #2
	ldr r2, =0x3333
	and r0, r2
	and r1, r2

	add r0, r0, r1
	lsr r1, r0, #4
	ldr r2, =0x0F0F
	and r0, r2
	and r1, r2

	add r0, r0, r1
	lsr r1, r0, #8
	ldr r2, =0x00FF
	and r0, r2
	and r1, r2

	add r0, r0, r1

	bx lr

main:
	ldr r0, =X
	ldr r1, =Y
	bl hamm
	ldr r2, =result
	str r0, [r2]

L: b L
