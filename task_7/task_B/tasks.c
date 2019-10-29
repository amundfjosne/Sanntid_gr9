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

#define PRIORITY					1
#define MS_TO_NS(x)					x*1000*1000
#define MS_TO_US(x)					x*1000


struct responseTaskArgs {
	long channel;
	long sleep_time;
	long busy_wait_time;
};

struct responseTaskArgs wait_args_L;
struct responseTaskArgs wait_args_M;
struct responseTaskArgs wait_args_H;

RT_TASK task_L;
RT_TASK task_M;
RT_TASK task_H;
RT_TASK ctrl;

RT_SEM sem1;
RT_SEM resource;


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
	rt_printf("Task %ld started\n", a.channel);

	rt_sem_p(&sem1, 10000000000);

	
	if (a.channel == 1) //Low
	{
		rt_printf("Task L released\n");
		rt_sem_inquire(&resource, &info1);
		rt_printf("Resource before L has count = %ld\n", info1.count);
		retval_sem_p_L = rt_sem_p(&resource, TM_INFINITE); // Lock the resource
		rt_sem_inquire(&resource, &info1);
		rt_printf("Task L locked resource with retval =  %ld\n", retval_sem_p_L);
		rt_printf("Resource after L has count = %ld\n", info1.count);

		busy_wait_us(MS_TO_US(a.busy_wait_time)); // Busy wait
		rt_sem_v(&resource); // Unlock the resource
		rt_printf("Task L unlocked resource\n");

		rt_printf("Task L finished\n");	
	}

	if (a.channel == 2) //Medium
	{
		rt_printf("Task M released\n");

		rt_task_sleep(MS_TO_NS(a.sleep_time)); // Sleep
		busy_wait_us(MS_TO_US(a.busy_wait_time)); // Busy wait

		rt_printf("Task M finished\n");	
	}

	if (a.channel == 3) //High
	{
		rt_printf("Task H released\n");

		rt_task_sleep(MS_TO_NS(a.sleep_time)); // Sleep
		rt_sem_inquire(&resource, &info2);
		rt_printf("Resource before H has count = %ld\n", info2.count);
		retval_sem_p_H = rt_sem_p(&resource, TM_INFINITE); // Lock the resource
		rt_sem_inquire(&resource, &info2);
		rt_printf("Task H locked resource with retval =  %ld\n", retval_sem_p_H);
		rt_printf("Resource after H has count = %ld\n", info2.count);

		busy_wait_us(MS_TO_US(a.busy_wait_time)); // Busy wait
		rt_sem_v(&resource); // Unlock the resource
		rt_printf("Task H unlocked resource\n");

		rt_printf("Task H finished\n");	
	}	

}



int main(){
	/*******************************************************
	****			     		Init	  				****
	*******************************************************/
	rt_print_auto_init(1);
	mlockall(MCL_CURRENT | MCL_FUTURE);
	set_cpu(1);

	wait_args_L.channel = 1;
	wait_args_L.sleep_time = 0;
	wait_args_L.busy_wait_time = 300;
	
	wait_args_M.channel = 2;
	wait_args_M.sleep_time = 100;
	wait_args_M.busy_wait_time = 500;

	wait_args_H.channel = 3;
	wait_args_H.sleep_time = 200;
	wait_args_H.busy_wait_time = 200;
	

	rt_printf("Starting...\n");

	/*******************************************************
	****			     Create threads  				****
	*******************************************************/

	rt_task_shadow(&ctrl, "ctrl", PRIORITY+10, T_CPU(1));
	
	/* Semaphore creation */
	rt_sem_create(&sem1, "sem1", 0, S_FIFO);
	rt_sem_create(&resource, "resource", 1, S_FIFO);

	/* Task creation */
	rt_task_create(&task_L, "task L", 0, PRIORITY+0, T_CPU(1)|T_JOINABLE);
	rt_task_create(&task_M, "task M", 0, PRIORITY+1, T_CPU(1)|T_JOINABLE);
	rt_task_create(&task_H, "task H", 0, PRIORITY+2, T_CPU(1)|T_JOINABLE);

	rt_task_start(&task_L, &waitTask, &wait_args_L);
	rt_task_start(&task_M, &waitTask, &wait_args_M);
	rt_task_start(&task_H, &waitTask, &wait_args_H);


	rt_task_sleep(MS_TO_NS(100));
	rt_sem_broadcast(&sem1);
	rt_task_sleep(MS_TO_NS(100));

	/* Task joining */
	rt_task_join(&task_L);
	rt_task_join(&task_M);
	rt_task_join(&task_H);

	/* Semaphore deletion */
	rt_sem_delete(&sem1);
	rt_sem_delete(&resource);

	rt_printf("Finished!\n");
}
