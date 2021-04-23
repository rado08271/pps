//
// Created by rafig on 30. 3. 2021.
//
#include <sys/time.h>
#include <stdio.h>
#include <stdbool.h>

#include "simple_timer.h"

struct timeval start, stop;
bool started, stopped;
double msResult = 0;
double tmpTime = 0;

long startTimer() {
    printf("Timer started\n");
    gettimeofday(&start, NULL);
    started = true;

    return start.tv_usec /1000;
}

long stopTimer() {
    printf("Timer stopped\n");
    gettimeofday(&stop, NULL);
    stopped = true;

    return stop.tv_usec /1000;
}

long getTimerResult() {
    if (!started || !stopped) return -1;

    msResult = (double)(stop.tv_usec - start.tv_usec)  + (double)(stop.tv_sec - start.tv_sec) * 1000000.0f;
    return (long) msResult;
}

