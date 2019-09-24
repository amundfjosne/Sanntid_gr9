//usr/bin/clang "$0" -o task_f_deadlock -std=gnu11 -g -O3 -lpthread -fsanitize=thread  && exec ./task_f_deadlock
//-fsanitize=thread
#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>

#define N 5 // Number of philos
int phil[N] = { 0, 1, 2, 3, 4 };
pthread_mutex_t forks[N];
long i = 0;

void* fn(void* args){

	while (1) {


	    int* num = args;
	    int left_fork = 0;
		int right_fork = ((*num)+1)%5;
	    //printf("%i\n",*num );
        if ((*num) <= 0)
		{
	    	left_fork = N - 1;
        }
        else
	    {
            left_fork = ((*num) - 1);
	    }

        printf("Philos %i is waiting for forks \n", *num);
        pthread_mutex_lock(&forks[left_fork]); // Taken left fork
        printf("Philos %i has left fork %i\n", *num, left_fork);
	    pthread_mutex_lock(&forks[right_fork]); // Taken right fork

        printf("Philos %i has right fork %i\n", *num, right_fork);


	    //sleep(1);

	    pthread_mutex_unlock(&forks[left_fork]); // Taken left fork

	    pthread_mutex_unlock(&forks[((*num)+1)%5]); // Taken right fork
        printf("Philos %i has released forks %i and %i\n", *num, left_fork, right_fork );


    }

    return NULL;

}




int main(){
    pthread_t thread_id[N];

	for (int i = 0; i < N; i++) {
		pthread_mutex_init(&forks[i], NULL);
	}


	for (int i = 0; i < N; i++) {

        // create philosopher processes
        pthread_create(&thread_id[i], NULL,
                       fn, &phil[i]);

        //printf("Philosopher %d is created\n", i);
    }







	for (int i = 0; i < N; i++)
	{

	    pthread_join(thread_id[i], NULL);
    }

    printf("%ld\n", i);

	for (int i = 0; i < N; i++)
	{

		pthread_mutex_destroy(&forks[i]);
	}

}
