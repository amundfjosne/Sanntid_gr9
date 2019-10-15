#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <sched.h>
#include <unistd.h>

#include "io.h"

struct timespec timespec_normalized(time_t sec, long nsec);

struct timespec timespec_add(struct timespec lhs, struct timespec rhs);
