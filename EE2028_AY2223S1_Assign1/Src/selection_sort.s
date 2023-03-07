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

@ R0 pointer to start of array
@ R1 length of array converted to end of array

@ R2 current value
@ R3 smallest value

@ R4 current pointer
@ R5 pointer to smallest value

@ R6 number of iterations (though this is constant lmao)

//C version:
@#define m 5
@array[m] = [5,4,3,2,1];
@int* i = &array[0];
@do{
@	int smallest_num = array[0];
@	int* smallest_ptr = &array[0];
@	if (*i < smallest_num)
@} while (i != &array[m];

@ write your program from here:
selection_sort:
	@store registers into stack
	PUSH {R4,R5,R6,R14}
	@ reset the iteration register
	MOV R6,#0

	@ multiply R1 by 4 to get address offset of last number in register
	MOV R2, #4
	MUL R1, R2
	@ add the offset to the pointer of the start of the array to get the pointer
	@to the last number in the array (to avoid using another register for the offset)
	ADD R1, R0
	@ subroutine now has R0 as start of array and R1 as end of array
	BL SUBROUTINE

	@//todo: calculate/evaluate number of iterations somehow
	MOV R0,R6

	@ pop back registers from the stack
	POP {R4,R5,R6,R14}
	@ go back to main function
	BX R14


SUBROUTINE:
	@start of checking, store the first value into the smallest number register
	LDR R3, [R0]
	@store the pointer to the smallest number here first
	MOV R4, R0

	check_array:
		@shift current pointer up one position
		ADD R4, #4
		@load the value in the pointer into R2
		LDR R2, [R4]
		@check if R2 is smaller then R3
		CMP R2,R3
		@branch to SMALLER_NUM if R2 is smaller
		BLT SMALLER_NUM

		SMALLER_NUM:
			@set the smallest value to the current value being checked
			MOV R3, R2
			@set the pointer to the smallest value
			MOV R5, R4
	@check if at the end of array
	CMP R4, R1
	@loop back to check_array if not at the end of array
	BNE check_array

	@//todo: swap the smallest values


	@by here the R0th value should be the smallest value
	@shift the value of R0 up by one position
	@function directly increments R0 as the "i" in the for loop
	ADD R0, #4
	@loop back to SUBROUTINE if not yet at last position
	CMP R0, R1
	BNE SUBROUTINE

	@return back to selection_sort
	BX R14



