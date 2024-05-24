#include "monitor.h"
#include "supplier.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

// function prototypes
void logSupplierAdded(char* czSupplierName, int iSupplySize);
void logSupplierWait(char* czSupplierName);

void* runSupplier(void* param)
{
	// Open the configuration file
    char *temp ;
    temp = strtok(param, "\r");  
	FILE* configFile = fopen(temp, "r"); //Open configuration file as read.
	if (!configFile)
	{
		fprintf(stderr, "Error opening configuration file: %s\n", strerror(errno));
		return NULL;
	}

    char *buffer;
	size_t bufferSize = 1024;
	buffer = (char *)malloc(bufferSize * sizeof(bufferSize));
	char name[1024];
	int supplyInterval =0;
	int repetitions=0;
    int supplySize=0;

	for(int i=0;i<4;i++)
	{
        fgets(buffer, bufferSize, configFile); //Read the lines from the txt file and put it into buffer.
        buffer[strcspn(buffer,"\r\n")]=0;
		switch(i){
			case 0:
                buffer = strtok(buffer, "\n");
				strcpy(name,buffer); //Copy name from buffer
				break;
			case 1:
				supplyInterval = atoi(buffer); //Covert the line of the supply interval in the txt file from string to int.
				break;
			case 2:
				repetitions = atoi(buffer); //Covert the line of the repetitions in the txt file from string to int.
				break;
            case 3:
                supplySize = atoi(buffer); //Covert the line of the supply size in the txt file from string to int.
                break;
		}
	} 
	fclose(configFile); // end reading file and close
    fprintf(stdout,"Initialized supplier: %s\n",name);
    fprintf(stdout,"Supplier %s has period %d seconds\n",name,supplyInterval);
    fprintf(stdout,"Supplier %s has %d iterations.\n",name,repetitions);
    fprintf(stdout,"Supplier %s supplies %d units at a time.\n",name,supplySize);

	// Supply when we should, wait when we need to supply but can't
	
		for (int i = 0; i < repetitions; i++)
		{
            pthread_mutex_lock(&theMonitor->mutex); //Lock the mutex to ensure that only one thread can access the shared resource.
			while ((theMonitor->maxCount-theMonitor->count) < 0 ) //A loop that will continue to execute as long as the difference between maxCount and count is less than 0.
			{
                logSupplierWait(name);	//Send a wait message to the log to print it out to the terminal.
				pthread_cond_wait(&theMonitor->supplier_cond, &theMonitor->mutex); //wait until the consumer signals that the supplier can keep trying to adding units.
				
			}
			
			theMonitor->count += supplySize; //Adds the supply size that was supplied to the total stock.
			logSupplierAdded(name, supplySize); //Send a message to the log that the supplier added units.
			pthread_cond_broadcast(&theMonitor->consumer_cond); //Wake up all the waiting consumer threads.
            pthread_mutex_unlock(&theMonitor->mutex); //Unlock the lock so other threads can acces the shared resource.
			sleep(supplyInterval); //Sleep some time before the next action as written in the txt file.
		}
        fprintf(stdout,"Supplier %s completed\n",name);

	return NULL;
}

/* Adds a log line that the supplier added new units to the machine
	Parameters:
	czSupplierName - the name of the supplier
	iSupplySize - the number of units supplied
*/
void logSupplierAdded(char* czSupplierName, int iSupplySize)
{
    time_t current_time = time(NULL); //Using the time() function to get the current time and store it in a variable called current_time.
    char* time_string = ctime(&current_time); //Using the ctime() function to convert the value stored in current_time to a string representation of the local time.
    time_string = strtok(time_string, "\n"); //Gets rid of unwanted new lines.
	//Print a log line about supplying
	printf("%s %s supplied %d units. Stock after = %d\n", time_string, czSupplierName, iSupplySize, theMonitor->count);
	return;
}

/* Adds a log line that the supplier is going to sleep
	Parameters:
	czSupplierName - the name of the supplier
*/
void logSupplierWait(char* czSupplierName)
{
	// Print a log line about going to wait
    time_t current_time = time(NULL);
    char* time_string = ctime(&current_time);
    time_string = strtok(time_string, "\n");    
	printf("%s %s going to wait.\n", time_string,czSupplierName);
	return;
}

// 5783
