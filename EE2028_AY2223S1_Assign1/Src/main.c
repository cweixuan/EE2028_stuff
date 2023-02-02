/**
 ******************************************************************************
 * @project        : EE2028 Assignment 1 Program Template
 * @file           : main.c
 * @author         : Ni Qingqing, ECE, NUS
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
 */

#include "stdio.h"

#define M 5	 // No. of numbers in array

// function to swap the the position of two elements
void swap(int *a, int *b) {
  int temp = *a;
  *a = *b;
  *b = temp;
}

// Selection sort implementation in C
int selection_sort_C(int arr[], int size) {
	int itr = 0;
	for (int i = 0; i < size - 1; i++) {
		int min_idx = i;
		for (int j = i + 1; j < size; j++) {
			// Locate the minimum element in each loop.
			if (arr[j] < arr[min_idx])
				min_idx = j;
		}
		// put min at the correct position
		swap(&arr[min_idx], &arr[i]);
		itr++;
	}
	return itr;
}

// function to print an array
void printArray(int arr[], int size, int itr) {
	printf("After %d iterations, the array is sorted as: \n{ ", itr);
	for (int i = 0; i < size; ++i) {
		printf("%d  ", arr[i]);
	}
	printf("}\n");
}


// Necessary function to enable printf() using semihosting
extern void initialise_monitor_handles(void);

// Functions to be written
extern int selection_sort(int* arg1, int arg2);

int main(void)
{
	// Necessary function to enable printf() using semihosting
	initialise_monitor_handles();

	int arr[M] = {20, 12, 10, 15, 2};
	int iteration;

	// sort with selection_sort.s
	iteration = selection_sort((int*)arr, (int)M);
	printf("output from slection_sort.s: \n");
	printArray(arr, M, iteration);



	//sort with slection_sort_C:
	int arr_C[M] = {20, 12, 10, 15, 2};
	iteration = selection_sort_C(arr_C, M);
	printf("\n \noutput from slection_sort_C: \n");
	printArray(arr_C, M, iteration);

}



