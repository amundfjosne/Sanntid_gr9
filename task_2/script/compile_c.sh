#!/bin/bash
cd ../task_c_container
clang -lpthread -g -fsanitize=thread -o task_c task_c.c
./task_c
cd ../script
