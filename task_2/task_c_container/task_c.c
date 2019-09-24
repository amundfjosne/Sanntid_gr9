#include <pthread.h>
#include <stdio.h>
#include <semaphore.h>

// Note the argument and return types: void*

long i = 0;
sem_t sem_1;


#define COUNTER_LIMIT 50000000 // 50 million

void* fn(void* args){
    //Now with semaphore


    long i_local = 0;

    sem_wait(&sem_1);
	for(i_local; i_local<COUNTER_LIMIT; i_local++)
    {

        i ++;

	}
    sem_post(&sem_1);
    printf("Local i = %ld\n", i_local);
    return NULL;
}


int main(){
    pthread_t threadHandle_1, threadHandle_2;

    sem_init(&sem_1, 0, 1);

    pthread_create(&threadHandle_1, NULL, fn, NULL);
	pthread_create(&threadHandle_2, NULL, fn, NULL);

    pthread_join(threadHandle_1, NULL);
    pthread_join(threadHandle_2, NULL);

	printf("Global i = %ld\n", i);
}
