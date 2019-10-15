// Command for reading serial:
// sudo cat /dev/ttyS0

#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <sched.h>
#include <unistd.h>

#include "io.h"
#include "timespec.h"

#define CH1 1
#define CH2 2
#define CH3 3

#define LOW  0
#define HIGH 1

#define NUMBER_OF_DISTURBANCE_TASKS 10

//#define TASK_A
//#define TASK_B
#define DISTURBANCES //Task C and Task D
#define TASK_D

struct responseTaskArgs {
	long channel;
};

int set_cpu(int cpu_number){
	cpu_set_t cpu;
	CPU_ZERO(&cpu);
	CPU_SET(cpu_number, &cpu);

	return pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpu);
}

void* responseTask(void* args)
{
	struct responseTaskArgs a = *(struct responseTaskArgs*)args;
	while(1){

		if (!io_read(a.channel))
		{
			io_write(a.channel, LOW);
			sleep(0);
			io_write(a.channel, HIGH);
		}
		sleep(0);
	}
    return NULL;
}

/// Periodic execution example ///
void* responseTaskPeriodic(void* args)
{

	struct timespec waketime;
	clock_gettime(CLOCK_REALTIME, &waketime);

	struct timespec period = {.tv_sec = 0, .tv_nsec = 1000000};

	struct responseTaskArgs a = *(struct responseTaskArgs*)args;
	printf("Periodic task %ld started\n", a.channel);
	while(1)
	{
		if (!io_read(a.channel))
		{
			io_write(a.channel, LOW);
			usleep(5);
			io_write(a.channel, HIGH);
		}

		// sleep
	    waketime = timespec_add(waketime, period);
	    clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &waketime, NULL);
	}
}


void* disturbanceTask(void* args)
{
	while(1)
	{
		asm volatile("" ::: "memory");
	}
}


int main(){
	/*******************************************************
	****			     Print task 	  				****
	*******************************************************/
#	ifdef TASK_A
	printf("Task A\n");
#	endif

#	ifdef TASK_B
	printf("Task B\n");
#	endif

#	ifdef TASK_D
	printf("Task D\n");
#	ifdef DISTURBANCES
		printf("With disturbances\n");
#	endif
#	else
#	ifdef DISTURBANCES
		printf("Task C\n");
#	endif
#	endif

	/*******************************************************
	****			     		Init	  				****
	*******************************************************/
	io_init();
#	ifndef TASK_A
	set_cpu(1);
#	endif

	printf("Starting...\n");

    pthread_t threadHandle_A, threadHandle_B, threadHandle_C;
	pthread_t disturbanceHandle[10];

	struct responseTaskArgs args_A;
	struct responseTaskArgs args_B;
	struct responseTaskArgs args_C;

	args_A.channel = CH1;
	args_B.channel = CH2;
	args_C.channel = CH3;

	/*******************************************************
	****			     Create threads  				****
	*******************************************************/
# 	ifdef TASK_D
	pthread_create(&threadHandle_A, NULL, responseTaskPeriodic, &args_A);
	pthread_create(&threadHandle_B, NULL, responseTaskPeriodic, &args_B);
	pthread_create(&threadHandle_C, NULL, responseTaskPeriodic, &args_C);
#	else
	pthread_create(&threadHandle_A, NULL, responseTask, &args_A);
	pthread_create(&threadHandle_B, NULL, responseTask, &args_B);
	pthread_create(&threadHandle_C, NULL, responseTask, &args_C);
#	endif


#	ifdef DISTURBANCES
	for (int i = 0; i < NUMBER_OF_DISTURBANCE_TASKS; i++)
	{
		printf("DisturbanceTask %d created\n", i);
		pthread_create(&disturbanceHandle[i], NULL, disturbanceTask, NULL);
	}
#	endif



	/*******************************************************
	****			      Join threads  				****
	*******************************************************/

    pthread_join(threadHandle_A, NULL);
    pthread_join(threadHandle_B, NULL);
	pthread_join(threadHandle_C, NULL);

#	ifdef DISTURBANCES
	for (int i = 0; i < NUMBER_OF_DISTURBANCE_TASKS; i++)
	{
		pthread_join(disturbanceHandle[i], NULL);
		printf("DisturbanceTask %d joined\n", i);
	}
#	endif



	printf("Finished!\n");
}
