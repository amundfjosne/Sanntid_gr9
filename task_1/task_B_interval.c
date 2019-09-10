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
	int ns_max = 50;
    int histogram[ns_max];
    memset(histogram, 0, sizeof(int)*ns_max);
	unsigned long long initial, current;



	for(int i = 0; i < 10*1000*1000; i++){
		initial = rdtsc();
        current = rdtsc();

		int ns = (current- initial)/2.6 ; // Difference in ns

		if(ns >= 0 && ns < ns_max)
		{
            histogram[ns]++;
        }

	}

	for(int i = 0; i < ns_max; i++)
	{
		printf("%d\n", histogram[i]);
	}


}

void task_b_clock_gettime() {
	int ns_max = 50;
    int histogram[ns_max];
    memset(histogram, 0, sizeof(int)*ns_max);
	struct timespec initial, current;


	for(int i = 0; i < 10*1000*1000; i++){


		clock_gettime(CLOCK_MONOTONIC, &initial);
        clock_gettime(CLOCK_MONOTONIC, &current);

		int ns = (current.tv_nsec - initial.tv_nsec); // Difference in ns

		if(ns >= 0 && ns < ns_max)
		{
            histogram[ns]++;
        }

	}

	for(int i = 0; i < ns_max; i++)
	{
		printf("%d\n", histogram[i]);
	}


}




void task_b_times() {
	int ns_max = 50;
    int histogram[ns_max];
    memset(histogram, 0, sizeof(int)*ns_max);

	struct tms initial, current;
	double t1, t2;


	for(int i = 0; i < 10*1000*1000; i++){


        t1 = (double)times(&initial);
		t2 = (double)times(&current);


		int ns = (int)(t2 - t1)/(10.0*1000.0*1000.0); // Difference in ns

		if(ns >= 0 && ns < ns_max)
		{
            histogram[ns]++;
        }

	}

	for(int i = 0; i < ns_max; i++)
	{
		printf("%d\n", histogram[i]);
	}

}




void main(void) {
	//task_b_rdtsc();
	//task_b_clock_gettime();
	//task_b_times();
}
