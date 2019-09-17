#include <assert.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/times.h>
#include <time.h>
#include <unistd.h>
#include <x86intrin.h>
#include <sched.h>

void task_D(void) {

	long xy_size  =    1000*1000*1000; // 8 GB (sizeof(long) = 8 bytes)
	long x_dim    =    1000;
	long y_dim    =    xy_size/x_dim;

	long** matrix = malloc(y_dim*sizeof(long*)); // Allocates memory

	for(long y = 0; y < y_dim; y++)
	{
	    matrix[y] = malloc(x_dim*sizeof(long));  // memory pointer
		//memset(matrix[y], 0, x_dim*sizeof(long)); // Fuck shit up?
	}

	printf("Allocation complete (press any key to continue...)\n");
	getchar();
}


void main(void) {
	task_D();
}
