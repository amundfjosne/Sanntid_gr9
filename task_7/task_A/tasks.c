/ Command for reading serial:
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

#define CH1 1
#define CH2 2
#define CH3 3

#define LOW  0
#define HIGH 1

#define NUMBER_OF_DISTURBANCE_TASKS 10
#define PRIORITY					1
#define SLEEP_PERIOD				1000000 //1ms

struct responseTaskArgs {
	long channel;
};

struct responseTaskArgs wait_args_1;
struct responseTaskArgs wait_args_2;

RT_TASK task1;
RT_TASK task2;
RT_TASK ctrl;

RT_SEM sem1;


int set_cpu(int cpu_number){
	cpu_set_t cpu;
	CPU_ZERO(&cpu);
	CPU_SET(cpu_number, &cpu);

	return pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpu);
}

void waitTask(void* args)
{
	struct responseTaskArgs a = *(struct responseTaskArgs*)args;
	rt_printf("Wait task %ld started\n", a.channel);

	rt_sem_p(&sem1, 10000000000);

	rt_printf("Wait task %ld released\n", a.channel);
	unsigned long duration = 10000000000;  // 10 second timeout
	unsigned long endTime = rt_timer_read() + duration;
	

}



int main(){
	/*******************************************************
	****			     		Init	  				****
	*******************************************************/
	rt_print_auto_init(1);
	mlockall(MCL_CURRENT | MCL_FUTURE);
	set_cpu(1);

	wait_args_1.channel = 1;
	wait_args_2.channel = 2;


	rt_printf("Starting...\n");

	/*******************************************************
	****			     Create threads  				****
	*******************************************************/

	rt_task_shadow(&ctrl, "ctrl", PRIORITY+2, T_CPU(1));

	rt_sem_create(&sem1, "sem1", 0, S_FIFO);

	rt_task_create(&task1, "task 1", 0, PRIORITY, T_CPU(1)|T_JOINABLE);
	rt_task_create(&task2, "task 2", 0, PRIORITY+1, T_CPU(1)|T_JOINABLE);

	rt_task_start(&task1, &waitTask, &wait_args_1);
	rt_task_start(&task2, &waitTask, &wait_args_2);

	rt_task_sleep(SLEEP_PERIOD*1000);

	rt_sem_broadcast(&sem1);

	rt_task_sleep(SLEEP_PERIOD*1000);

	rt_task_join(&task1);
	rt_task_join(&task2);

	rt_sem_delete(&sem1);

	rt_printf("Finished!\n");
}
