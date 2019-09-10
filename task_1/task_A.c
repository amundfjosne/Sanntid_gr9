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





struct timespec timespec_normalized(time_t sec, long nsec){
    while(nsec >= 1000000000){
        nsec -= 1000000000;
        ++sec;
    }
    while(nsec < 0){
        nsec += 1000000000;
        --sec;
    }
    return (struct timespec){sec, nsec};
}

struct timespec timespec_sub(struct timespec lhs, struct timespec rhs){
    return timespec_normalized(lhs.tv_sec - rhs.tv_sec, lhs.tv_nsec - rhs.tv_nsec);
}

struct timespec timespec_add(struct timespec lhs, struct timespec rhs){
    return timespec_normalized(lhs.tv_sec + rhs.tv_sec, lhs.tv_nsec + rhs.tv_nsec);
}

int timespec_cmp(struct timespec lhs, struct timespec rhs){
    if (lhs.tv_sec < rhs.tv_sec)
        return -1;
    if (lhs.tv_sec > rhs.tv_sec)
        return 1;
    return lhs.tv_nsec - rhs.tv_nsec;
}





void busy_wait(struct timespec t){
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    struct timespec then = timespec_add(now, t);

    while(timespec_cmp(now, then) < 0){
        for(int i = 0; i < 10000; i++){}
        clock_gettime(CLOCK_MONOTONIC, &now);
    }
}




void task_a_sleep(void)
{
	sleep(1);
	printf("Slept for 1 second using sleep\n" );
}

void task_a_sleep_busy_wait(void) {
    struct timespec t;
	t.tv_sec = 1;
	busy_wait(t);

	printf("Slept for 1 second using busy_wait\n" );
}

void task_a_sleep_busy_wait_using_times(void) {
	struct tms start, later;
	//clock_t start_time = times(&start); // Initial Time

    for (size_t i = 0; i < 1; i++)
	{
		clock_t start_time = times(&start); // Initial Time
		while (((times(&later) - start_time)/100) < 1) {
			//printf("%lld\n", (long long)(times(&later)));
		}
    }


    printf("Slept for 1 second using busy_wait with times function\n" );
}




int main(void){
	/* code */
	//task_a_sleep();
	task_a_sleep_busy_wait();
    //task_a_sleep_busy_wait_using_times();
	return 0;
}
