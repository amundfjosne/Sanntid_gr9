#!/bin/bash
cd ../task_a_container
clang -lpthread -o task_a task_a.c
./task_a

echo "Press any key to continue"
while [ true ] ; do
    read -t 3 -n 1
    if [ $? = 0 ] ; then
        exit ;
    fi
done
clang -lpthread -g - fsanitize=thread -o task_a task_a.c
./task_a
cd ../script
