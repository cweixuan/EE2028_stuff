/*
 * selection_sort.s
 *
 *  Created on: 19/1/2022
 *      Author: Ni Qingqing
 */
   .syntax unified
	.cpu cortex-m4
	.fpu softvfp
	.thumb

		.global selection_sort

@ Start of executable code
.section .text

@ EE2028 Assignment 1, Sem 2, AY 2022/23
@ (c) ECE NUS, 2023
@ Selection sort arr in ascending order

@ Write Student 1’s Name here:
@ Write Student 2’s Name here:

@ You could create a look-up table of registers here:

@ R0 ...
@ R1 ...

@ write your program from here:
selection_sort:
	PUSH {R14}

	BL SUBROUTINE

	POP {R14}
	BX LR

SUBROUTINE:

	BX LR
