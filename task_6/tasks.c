// Command for reading serial:
// sudo cat /dev/ttyS0

/* Task 5 headers */
#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <sched.h>
#include <unistd.h>

/* Task 6 headers */
#include <native/task.h>
#include <native/timer.h>
#include <sys/mman.h>
#include <rtdk.h>


#include "io.h"
#include "timespec.h"

#define CH1 1
#define CH2 2
#define CH3 3

#define LOW  0
#define HIGH 1

#define NUMBER_OF_DISTURBANCE_TASKS 10
#define PRIORITY					0
#define PERIOD_ns					1000000

//#define PERIODIC
#define DISTURBANCES

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
	rt_printf("Busy wait task %ld started\n", a.channel);
	unsigned long duration = 100000000000;  // 100 second timeout
	unsigned long endTime = rt_timer_read() + duration;
	
	while(1){
		if (!io_read(a.channel))
		{
			io_write(a.channel, LOW);
			rt_timer_spin(5000000);
			io_write(a.channel, HIGH);
		}
	    if(rt_timer_read() > endTime){
	        rt_printf("Time expired\n");
	        rt_task_delete(NULL);
	    }
	    if(rt_task_yield()){
	        rt_printf("Task failed to yield\n");
	        rt_task_delete(NULL);
	    }
	}
}

void* responseTaskPeriodic(void* args)
{

	struct responseTaskArgs a = *(struct responseTaskArgs*)args;
    rt_printf("Periodic task %ld started\n", a.channel);
    unsigned long duration = 100000000000;  // 100 second timeout
	unsigned long endTime = rt_timer_read() + duration;
	while(1){
		//rt_printf("Periodic task %ld working\n", a.channel);
		if (!io_read(a.channel))
		{
			io_write(a.channel, LOW);
			rt_timer_spin(5000000);
			io_write(a.channel, HIGH);
		}
	    if(rt_timer_read() > endTime){
	        rt_printf("Time expired\n");
	        rt_task_delete(NULL);
	    }
	    if(rt_task_yield()){
	        rt_printf("Task failed to yield\n");
	        rt_task_delete(NULL);
	    }
	    rt_task_wait_period(NULL);
	}
	
}

void* disturbanceTask(void* args)
{
	while(1)
	{
		asm volatile("" ::: "memory");
	}
}

struct responseTaskArgs args_A;
struct responseTaskArgs args_B;
struct responseTaskArgs args_C;

RT_TASK task1;
RT_TASK task2;
RT_TASK task3;


int main(){
	/*******************************************************
	****			     		Init	  				****
	*******************************************************/
	rt_print_auto_init(1);
	mlockall(MCL_CURRENT | MCL_FUTURE);
	io_init();
	set_cpu(1);

	args_A.channel = CH1;
	args_B.channel = CH2;
	args_C.channel = CH3;
	pthread_t disturbanceHandle[10];


	rt_printf("Starting...\n");

	/*******************************************************
	****			     Create threads  				****
	*******************************************************/

	rt_task_create(&task1, "task 1", 0, PRIORITY, T_CPU(1));
	rt_task_create(&task3, "task 3", 0, PRIORITY, T_CPU(1));
	rt_task_create(&task2, "task 2", 0, PRIORITY, T_CPU(1));
#	ifndef PERIODIC
	rt_task_start(&task1, &responseTask, &args_A);
	rt_task_start(&task2, &responseTask, &args_B);
	rt_task_start(&task3, &responseTask, &args_C);
#	else
	// Change these
	rt_task_start(&task1, &responseTaskPeriodic, &args_A);
	rt_task_start(&task2, &responseTaskPeriodic, &args_B);
	rt_task_start(&task3, &responseTaskPeriodic, &args_C);
	rt_task_set_periodic(&task1, TM_NOW, PERIOD_ns);
	rt_task_set_periodic(&task2, TM_NOW, PERIOD_ns);
	rt_task_set_periodic(&task3, TM_NOW, PERIOD_ns);
#	endif

#	ifdef DISTURBANCES
	for (int i = 0; i < NUMBER_OF_DISTURBANCE_TASKS; i++)
	{
		rt_printf("DisturbanceTask %d created\n", i);
		pthread_create(&disturbanceHandle[i], NULL, disturbanceTask, NULL);
	}
#	endif


	pause();
	printf("Finished!\n");
}
