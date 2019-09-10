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

unsigned long long rdtsc(){
		unsigned int lo,hi;
		__asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));
		return ((unsigned long long)hi << 32) | lo;
}

void task_b_rdtsc() {
	unsigned long long initial, current;
	initial = rdtsc();
	for(int i = 0; i < 10*1000*1000; i++){
    rdtsc();
    }
	current = rdtsc();

	printf("Time average = %f\n", (current-initial)/(10*1000*1000)/(2.6));

}

void task_b_clock_gettime() {
	struct timespec now;
	unsigned long long initial, current;
	initial = rdtsc();
	for(int i = 0; i < 10*1000*1000; i++){
        clock_gettime(CLOCK_MONOTONIC, &now);
    }
	current = rdtsc();

	printf("Time average = %f\n", (current-initial)/(10*1000*1000)/(2.6));

}

void task_b_times() {
	struct tms start;
	unsigned long long initial, current;
	initial = rdtsc();
	for(int i = 0; i < 10*1000*1000; i++){
        times(&start);
    }
	current = rdtsc();

	printf("Time average = %f\n", (current-initial)/(10*1000*1000)/(2.6));

}




void main(void) {
	//task_b_rdtsc();
	//task_b_clock_gettime();
	task_b_times();
}
