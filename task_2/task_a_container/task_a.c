#include <pthread.h>
#include <stdio.h>
// Note the argument and return types: void*

long i = 0;

#define COUNTER_LIMIT 50000000 // 50 million

void* fn(void* args){

    long i_local = 0;

	for(i_local; i_local<COUNTER_LIMIT; i_local++)
    {
	    i ++;
	}

    printf("Local i = %ld\n", i_local);
    return NULL;
}


int main(){
    pthread_t threadHandle_1, threadHandle_2;

    pthread_create(&threadHandle_1, NULL, fn, NULL);
	pthread_create(&threadHandle_2, NULL, fn, NULL);

    pthread_join(threadHandle_1, NULL);
    pthread_join(threadHandle_2, NULL);

	printf("Global i = %ld\n", i);
}
