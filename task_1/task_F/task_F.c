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
#include <sched.h>

#include "array.h"


int main(void) {

	int array_length = 10;

	Array array_a = array_new(array_length);
    array_print(array_a);



	for(int i =0; i < array_length*5; i++ )
	{
         array_insertBack(&array_a, i);
	}
    array_print(array_a);
    array_destroy(array_a);
	return 0;

}
