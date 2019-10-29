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

/* Task 7 headers */
#include <native/sem.h>
#include <native/mutex.h>

#define PRIORITY		1
#define MS_TO_NS(x)		x*1000*1000
#define MS_TO_US(x)		x*1000

#define PRIORITY_L 		PRIORITY
#define PRIORITY_M 		PRIORITY + 1
#define PRIORITY_H 		PRIORITY + 2


struct responseTaskArgs {
	long priority;
	long busy_wait_time_1;
	long busy_wait_time_2;
	long busy_wait_time_3;
};

struct responseTaskArgs wait_args_L;
struct responseTaskArgs wait_args_M;
struct responseTaskArgs wait_args_H;

RT_TASK task_L;
RT_TASK task_M;
RT_TASK task_H;
RT_TASK ctrl;

RT_SEM   fence;
RT_MUTEX mutex_A;
RT_MUTEX mutex_B;


int set_cpu(int cpu_number){
	cpu_set_t cpu;
	CPU_ZERO(&cpu);
	CPU_SET(cpu_number, &cpu);

	return pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpu);
}

void busy_wait_us(unsigned long delay)
{

	for(; delay > 0; delay--)
    {
        rt_timer_spin(1000);
	}
}

RT_SEM_INFO info1;
RT_SEM_INFO info2;
int retval_sem_p_L;
int retval_sem_p_H;

void waitTask(void* args)
{
	struct responseTaskArgs a = *(struct responseTaskArgs*)args;
	rt_printf("Task %ld started\n", a.priority);

	rt_sem_p(&fence, 10000000000);

	
	if (a.priority == PRIORITY_L)
	{
		rt_printf("Task L waiting for A\n");
		rt_mutex_acquire(&mutex_A, TM_INFINITE); 			//Take mutex A
		rt_printf("Task L got A\n");

   		for(int printloop = 10; printloop > 0; printloop--) //Busy-wait for 3 time units
   		{
   			struct rt_task_info temp;
			rt_task_inquire(NULL, &temp);
			busy_wait_us(MS_TO_US(a.busy_wait_time_1)); // Busy wait
			rt_printf("Task L busy waiting with Base prio: %i and Current prio: %i\n", temp.bprio, temp.cprio);
		}

		rt_printf("Task L waiting for B\n");
		rt_mutex_acquire(&mutex_B, TM_INFINITE); 			//Take mutex B
		rt_printf("Task L got B\n");

   		for(int printloop = 10; printloop > 0; printloop--) //Busy-wait for 3 time units
   		{
   			struct rt_task_info temp;
			rt_task_inquire(NULL, &temp);
			busy_wait_us(MS_TO_US(a.busy_wait_time_2)); // Busy wait
			rt_printf("Task L busy waiting with Base prio: %i and Current prio: %i\n", temp.bprio, temp.cprio);
		}

		rt_mutex_release(&mutex_B); 						//Return B
		rt_printf("Task L released B\n");
		rt_mutex_release(&mutex_A); 						//Return A
		rt_printf("Task L released A\n");

		for(int printloop = 10; printloop > 0; printloop--) //Busy-wait for 1 time unit
   		{
   			struct rt_task_info temp;
			rt_task_inquire(NULL, &temp);
			busy_wait_us(MS_TO_US(a.busy_wait_time_3)); // Busy wait
			rt_printf("Task L busy waiting with Base prio: %i and Current prio: %i\n", temp.bprio, temp.cprio);
		}
	}

	if (a.priority == PRIORITY_H)
	{
		rt_task_sleep(MS_TO_NS(1)); 						// Sleep for 1 time unit

		rt_printf("Task H waiting for B\n");
		rt_mutex_acquire(&mutex_B, TM_INFINITE); 			//Take mutex B
		rt_printf("Task H got B\n");

   		for(int printloop = 10; printloop > 0; printloop--) //Busy-wait for 1 time unit
   		{
   			struct rt_task_info temp;
			rt_task_inquire(NULL, &temp);
			busy_wait_us(MS_TO_US(a.busy_wait_time_1)); // Busy wait
			rt_printf("Task L busy waiting with Base prio: %i and Current prio: %i\n", temp.bprio, temp.cprio);
		}

		rt_printf("Task H waiting for A\n");
		rt_mutex_acquire(&mutex_A, TM_INFINITE); 			//Take mutex A
		rt_printf("Task H got A\n");

   		for(int printloop = 10; printloop > 0; printloop--) //Busy-wait for 2 time units
   		{
   			struct rt_task_info temp;
			rt_task_inquire(NULL, &temp);
			busy_wait_us(MS_TO_US(a.busy_wait_time_2)); // Busy wait
			rt_printf("Task L busy waiting with Base prio: %i and Current prio: %i\n", temp.bprio, temp.cprio);
		}

		rt_mutex_release(&mutex_A); 						//Return A
		rt_printf("Task H released A\n");
		rt_mutex_release(&mutex_B); 						//Return B
		rt_printf("Task H released B\n");

		for(int printloop = 10; printloop > 0; printloop--) //Busy-wait for 1 time unit
   		{
   			struct rt_task_info temp;
			rt_task_inquire(NULL, &temp);
			busy_wait_us(MS_TO_US(a.busy_wait_time_3)); // Busy wait
			rt_printf("Task L busy waiting with Base prio: %i and Current prio: %i\n", temp.bprio, temp.cprio);
		}
	}

}



int main(){
	/*******************************************************
	****			     		Init	  				****
	*******************************************************/
	rt_print_auto_init(1);
	mlockall(MCL_CURRENT | MCL_FUTURE);
	set_cpu(1);

	wait_args_L.priority = PRIORITY_L;
	wait_args_L.busy_wait_time_1 = 3;
	wait_args_L.busy_wait_time_2 = 3;
	wait_args_L.busy_wait_time_3 = 1;

	wait_args_H.priority = PRIORITY_H;
	wait_args_H.busy_wait_time_1 = 1;
	wait_args_H.busy_wait_time_2 = 2;
	wait_args_H.busy_wait_time_3 = 1;
	

	rt_printf("Starting...\n");

	/*******************************************************
	****			     Create threads  				****
	*******************************************************/

	rt_task_shadow(&ctrl, "ctrl", PRIORITY+10, T_CPU(1));
	
	/* Semaphore creation */
	rt_sem_create(&fence, "fence", 0, S_FIFO);
	rt_mutex_create(&mutex_A, "mutex_A");
	rt_mutex_create(&mutex_B, "mutex_B");

	/* Task creation */
	rt_task_create(&task_L, "task L", 0, PRIORITY_L, T_CPU(1)|T_JOINABLE);
	rt_task_create(&task_H, "task H", 0, PRIORITY_H, T_CPU(1)|T_JOINABLE);

	rt_task_start(&task_L, &waitTask, &wait_args_L);
	rt_task_start(&task_H, &waitTask, &wait_args_H);

	rt_task_sleep(MS_TO_NS(100));
	rt_sem_broadcast(&fence);
	rt_task_sleep(MS_TO_NS(100));

	/* Task joining */
	rt_task_join(&task_L);
	rt_task_join(&task_H);

	/* Semaphore deletion */
	rt_sem_delete(&fence);
	rt_mutex_delete(&mutex_A);
	rt_mutex_delete(&mutex_B);

	rt_printf("Finished!\n");
}
