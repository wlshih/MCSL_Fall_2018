	.syntax unified
	.cpu cortex-m4
	.thumb

.text
	.global delay // void delay(int);

delay:
	push {r0, lr}
L:	subs r0, r0, #1
	bne L
	pop {r0, pc}
