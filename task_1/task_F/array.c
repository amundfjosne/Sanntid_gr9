#include <malloc.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "array.h"


// Construction / Destruction

Array array_new(long capacity){
    assert(capacity > 0);
    return (Array){malloc(sizeof(long)*capacity), 0, 0, capacity};
}

void array_destroy(Array a){
    free(a.data);
}


// Primitives

long array_empty(Array a){
    return a.back <= a.front;
}

long array_front(Array a){
    return a.data[a.front];
}

long array_back(Array a){
    return a.data[a.back - 1];
}

void array_popFront(Array* a){
    a->front++;
}

void array_popBack(Array* a){
    a->back--;
}

Array array_save(Array a){
    return (Array){a.data, a.front, a.back, a.capacity};
}


// Iteration

void array_foreach(Array a, void fn(long)){
    for(Array b = array_save(a); !array_empty(b); array_popFront(&b)){
        fn(array_front(b));
    }
}

void array_foreachReverse(Array a, void fn(long)){
    for(Array b = array_save(a); !array_empty(b); array_popBack(&b)){
        fn(array_back(b));
    }
}

static void _array_printSingleLongHelper(long i){
    printf(", %ld", i);
}

void array_print(Array a){
    printf("Array:{");
    if(!array_empty(a)){
        printf("%ld", array_front(a));
        array_popFront(&a);
    }
    array_foreach(a, _array_printSingleLongHelper);
    printf("}\n");
}


// Capacity

long array_length(Array a){
    return a.back - a.front;
}

void array_reserve(Array* a, long capacity){
    Array new_list = array_new(capacity);
    Array temp_array = array_save(*a);

    *a = array_save(new_list);

    for(long i =0; i<array_length(*a); i++)
    {
        array_insertBack(a, temp_array.data[i]);
    }


}


// Modifiers

void array_insertBack(Array* a, long stuff){
    long length_a = array_length(*a);
    long capacity = a->capacity;

    if( a->back >= (capacity-1))
    {
        printf("at end, reserving more space\n");
        array_reserve(a, capacity*2);


    }
    a->data[length_a] = stuff;
	a->back++;
    printf(" %ld\n", a->back);

}
