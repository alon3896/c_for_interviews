#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <ctype.h>
#include <string.h>
#include "monitor.h"
#include "supplier.h"
#include "consumer.h"

struct thread_info {
	pthread_t thread_id;
	int thread_num;
	char* argv_string;
};

monitor_t* theMonitor;
// initialize the monitor with the standard parameters
int monitor_init(monitor_t* monitor, int maxCount)
{
	int ret;

	// Initialize the mutex
	ret = pthread_mutex_init(&monitor->mutex, NULL);
	if (ret != 0)
	{
		fprintf(stderr, "Error initializing mutex: %s\n", strerror(ret));
        pthread_mutex_destroy(&monitor->mutex);
		return ret;
	}

	// Initialize the condition variables
	ret = pthread_cond_init(&monitor->supplier_cond, NULL);
	if (ret != 0)
	{
		fprintf(stderr, "Error initializing supplier condition variable: %s\n", strerror(ret));
        pthread_mutex_destroy(&monitor->mutex);
		return ret;
	}
	ret = pthread_cond_init(&monitor->consumer_cond, NULL);
	if (ret != 0)
	{
		fprintf(stderr, "Error initializing consumer condition variable: %s\n", strerror(ret));
        pthread_mutex_destroy(&monitor->mutex);
		return ret;
	}

	// Set the maximum count, the current count, and the consumed count
	monitor->maxCount = maxCount;
	monitor->count = 0;
	monitor->consumed = 0;

	// Set the global monitor pointer
	theMonitor = monitor;

	return 0;
}

// print out how to use the program
void showUsage()
{
	printf("Usage: vending maxUnits file1 [file2 ...]\nFile names must include \"consumer\" or \"supplier\" in their name\n");
}

// Run the vending machine logic
int main(int argc, char* args[])
{
	int maxCount = 0;
	const char* CONSUMER = "consumer";
	const char* SUPPLIER = "supplier";

	// check that we have the parameters we need
	if (argc < 3)
	{
		showUsage();
		return -1;
	}

	// parse the parameter
	maxCount = atoi(args[1]);

	// Initialize the monitor
	monitor_t monitor;
	int ret = monitor_init(&monitor, maxCount);
	if (ret != 0)
	{
		fprintf(stderr, "Error initializing monitor: %s\n", strerror(ret));
		return ret;
	}

	// how many suppliers and consumers we have
	int supplierCount = 0, consumerCount = 0;
    struct thread_info* supplier_thread = (struct thread_info*)malloc(sizeof(struct thread_info) * (argc - 2)); //allocating memory dynamically for an array of struct thread_info. The variable supplier_thread is a pointer to the first element of this array.
	struct thread_info* consumer_thread = (struct thread_info*)malloc(sizeof(struct thread_info) * (argc - 2)); //allocating memory dynamically for an array of struct thread_info. The variable consumer_thread is a pointer to the first element of this array.
	

	for (int i = 2; i < argc; i++)
	{
		char* configFileName = args[i];
		if (strstr(configFileName, CONSUMER))
		{
			consumerCount++;
            consumer_thread[consumerCount-1].thread_num = i;
			consumer_thread[consumerCount-1].argv_string = configFileName;
			// Launch a consumer thread!
			pthread_create(&consumer_thread[consumerCount-1].thread_id, NULL, runConsumer, (void*)configFileName);
		}
		else if (strstr(configFileName, SUPPLIER))
		{
			supplierCount++;
            supplier_thread[supplierCount-1].thread_num = i;
			supplier_thread[supplierCount-1].argv_string = configFileName;
			// Launch a supplier thread!
			pthread_create(&supplier_thread[supplierCount-1].thread_id, NULL, runSupplier, (void*)configFileName);
		}
		else {
			printf("Unrecognized file name (%s)- must contain \"consumer\" or \"supplier\" in the name\n", args[i]);
			exit(1);
		}
	}

	// Wait until all of them are done (if they will ever be done)
	// Wait for all consumer threads to finish
    for (int i = 0; i < consumerCount; i++) {
    pthread_join(consumer_thread[i].thread_id, NULL);
    }
    // Wait for all supplier threads to finish
    for (int i = 0; i < supplierCount; i++) {
    pthread_join(supplier_thread[i].thread_id, NULL);
    }
	// Print out the final state and completion message
    time_t current_time = time(NULL);
    char* time_string = ctime(&current_time);
    time_string = strtok(time_string, "\n");    
	printf("%s All threads completed. Total consumed = %d. Final stock = %d\n",time_string,theMonitor->consumed,theMonitor->count);

	exit(0);
}

// 5783
