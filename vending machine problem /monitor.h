#ifndef MONITOR_H
#define MONITOR_H

#include <pthread.h>

typedef struct {
    pthread_mutex_t mutex; //A mutex
    pthread_cond_t supplier_cond; //A supplier condition    
    pthread_cond_t consumer_cond; //A consumer condition
    int maxCount; //The maximum value.
    int count; //The counter of the total stock.
    int consumed; //The counter for the total consumed.

} monitor_t;

extern monitor_t* theMonitor;
#endif

// 5783
