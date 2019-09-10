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


void task_c_clock_gettime() {
	int ns_max = 300;
    int histogram[ns_max];
    memset(histogram, 0, sizeof(int)*ns_max);
	struct timespec initial, current;


	for(int i = 0; i < 10*1000*1000; i++){


		clock_gettime(CLOCK_MONOTONIC, &initial);
		sched_yield();
        clock_gettime(CLOCK_MONOTONIC, &current);

		int ns = (current.tv_nsec - initial.tv_nsec); // Difference in ns
        //printf("%d\n",ns );
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

void main(void)
{
	task_c_clock_gettime();
}
