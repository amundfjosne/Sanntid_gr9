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

#define PRIORITY_A  	PRIORITY_H + 1
#define PRIORITY_B      PRIORITY_H + 2


struct responseTaskArgs {
	long priority;
	long busy_wait_time_1;
	long busy_wait_time_2;
	long busy_wait_time_3;
};

struct resource {
	RT_MUTEX mutex;
	long priority;
};

struct resource resource_A;
struct resource resource_B;

RT_TASK task_L;
RT_TASK task_H;
RT_TASK ctrl;

RT_SEM   fence;


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
	struct rt_task_info task_info;
	
	if (a.priority == PRIORITY_L)
	{
		rt_printf("Task L waiting for A\n");
		rt_mutex_acquire(&resource_A.mutex, TM_INFINITE); 	//Take mutex A
		rt_task_set_priority(NULL, resource_A.priority);
		rt_task_inquire(NULL, &task_info);
		rt_printf("Task L got A with prio: %i\n", task_info.bprio);

   		for(int printloop = 10; printloop > 0; printloop--) //Busy-wait for 3 time units
   		{
			rt_task_inquire(NULL, &task_info);
			busy_wait_us(MS_TO_US(a.busy_wait_time_1)); // Busy wait
			rt_printf("Task L busy waiting with Base prio: %i and Current prio: %i\n", task_info.bprio, task_info.cprio);
		}

		rt_printf("Task L waiting for B\n");
		rt_mutex_acquire(&resource_B.mutex, TM_INFINITE); 	//Take mutex B
		rt_task_set_priority(NULL, resource_B.priority);
		rt_task_inquire(NULL, &task_info);
		rt_printf("Task L got B with prio: %i\n", task_info.bprio);


   		for(int printloop = 10; printloop > 0; printloop--) //Busy-wait for 3 time units
   		{
			rt_task_inquire(NULL, &task_info);
			busy_wait_us(MS_TO_US(a.busy_wait_time_2)); // Busy wait
			rt_printf("Task L busy waiting with Base prio: %i and Current prio: %i\n", task_info.bprio, task_info.cprio);
		}

		rt_mutex_release(&resource_B.mutex); 				//Return B
		rt_printf("Task L released B\n");
		rt_mutex_release(&resource_A.mutex); 				//Return A
		rt_printf("Task L released A\n");
		rt_task_set_priority(NULL, PRIORITY_L);

		rt_task_inquire(NULL, &task_info);
		rt_printf("Task L reset with Base prio: %i and Current prio: %i\n", task_info.bprio, task_info.cprio);
		for(int printloop = 10; printloop > 0; printloop--) //Busy-wait for 1 time unit
   		{
			rt_task_inquire(NULL, &task_info);
			busy_wait_us(MS_TO_US(a.busy_wait_time_3)); // Busy wait
			rt_printf("Task L busy waiting with Base prio: %i and Current prio: %i\n", task_info.bprio, task_info.cprio);
		}
	}

	if (a.priority == PRIORITY_H)
	{
		rt_task_sleep(MS_TO_NS(1)); 						// Sleep for 1 time unit
		rt_printf("Task H waiting for B\n");
		rt_mutex_acquire(&resource_B.mutex, TM_INFINITE); 	//Take mutex B
		rt_task_set_priority(NULL, resource_B.priority);
		rt_task_inquire(NULL, &task_info);
		rt_printf("Task H got B with prio: %i\n", task_info.bprio);

   		for(int printloop = 10; printloop > 0; printloop--) //Busy-wait for 1 time unit
   		{
			rt_task_inquire(NULL, &task_info);
			busy_wait_us(MS_TO_US(a.busy_wait_time_1)); // Busy wait
			rt_printf("Task H busy waiting with Base prio: %i and Current prio: %i\n", task_info.bprio, task_info.cprio);
		}

		rt_printf("Task H waiting for A\n");
		rt_mutex_acquire(&resource_A.mutex, TM_INFINITE); 	//Take mutex A
		rt_task_set_priority(NULL, resource_A.priority);
		rt_task_inquire(NULL, &task_info);
		rt_printf("Task H got A with prio: %i\n", task_info.bprio);

   		for(int printloop = 10; printloop > 0; printloop--) //Busy-wait for 2 time units
   		{
			rt_task_inquire(NULL, &task_info);
			busy_wait_us(MS_TO_US(a.busy_wait_time_2)); // Busy wait
			rt_printf("Task H busy waiting with Base prio: %i and Current prio: %i\n", task_info.bprio, task_info.cprio);
		}

		rt_mutex_release(&resource_A.mutex); 				//Return A
		rt_printf("Task H released A\n");
		rt_mutex_release(&resource_B.mutex); 				//Return B
		rt_printf("Task H released B\n");
		rt_task_set_priority(NULL, PRIORITY_H);

		rt_task_inquire(NULL, &task_info);
		rt_printf("Task H reset with Base prio: %i and Current prio: %i\n", task_info.bprio, task_info.cprio);
		for(int printloop = 10; printloop > 0; printloop--) //Busy-wait for 1 time unit
   		{
			rt_task_inquire(NULL, &task_info);
			busy_wait_us(MS_TO_US(a.busy_wait_time_3)); // Busy wait
			rt_printf("Task H busy waiting with Base prio: %i and Current prio: %i\n", task_info.bprio, task_info.cprio);
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

	struct responseTaskArgs wait_args_L = {
		.priority = PRIORITY_L,
		.busy_wait_time_1 = 3,
		.busy_wait_time_2 = 3,
		.busy_wait_time_3 = 1
	};
	struct responseTaskArgs wait_args_H = {
		.priority = PRIORITY_H,
		.busy_wait_time_1 = 1,
		.busy_wait_time_2 = 2,
		.busy_wait_time_3 = 1
	};
	resource_A.priority = PRIORITY_A;

	resource_B.priority = PRIORITY_B;

	rt_printf("Starting...\n");

	/*******************************************************
	****			     Create threads  				****
	*******************************************************/

	rt_task_shadow(&ctrl, "ctrl", PRIORITY+10, T_CPU(1));
	
	/* Semaphore creation */
	rt_sem_create(&fence, "fence", 0, S_FIFO);
	rt_mutex_create(&resource_A.mutex, "mutex_A");
	rt_mutex_create(&resource_B.mutex, "mutex_B");

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
	rt_mutex_delete(&resource_A.mutex);
	rt_mutex_delete(&resource_B.mutex);

	rt_printf("Finished!\n");
}
