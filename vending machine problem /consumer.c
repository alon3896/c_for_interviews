#include "monitor.h"
#include "consumer.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

// function prototypes
void logConsumerRemoved(char* czConsumerName, int iConsumeSize);
void logConsumerWait(char* czConsumerName);

void* runConsumer(void* param)
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
	int consumptionInterval =0;
	int repetitions=0;
    int consumeSize=0;
	for(int i=0;i<4;i++)
	{
        fgets(buffer, bufferSize, configFile); //Read the lines from the txt file and put it into buffer.
        buffer[strcspn(buffer,"\r\n")]=0;
		switch(i){
			case 0:
				strcpy(name,buffer); //Copy name from buffer
				break;
			case 1:
				consumptionInterval = atoi(buffer); //Covert the line of the consumption interval in the txt file from string to int.
				break;
			case 2:
				repetitions = atoi(buffer); //Covert the line of the repetitions in the txt file from string to int.
				break;
            case 3:
                consumeSize = atoi(buffer); //Covert the line of the consume size in the txt file from string to int.
                break;
		}
	} 
	fclose(configFile); // End reading file and close
    fprintf(stdout,"Initialized consumer: %s\n",name);
    fprintf(stdout,"Consumer %s has period %d seconds\n",name,consumptionInterval);
    fprintf(stdout,"Consumer %s has %d iterations.\n",name,repetitions);
    fprintf(stdout,"Consumer %s consumes %d units at a time.\n",name,consumeSize);

	// Consume when we should, wait when we need to consume but can't
    
    for (int i = 0; i < repetitions; i++) //A loop of the repetitions of the consumer.
    {
        pthread_mutex_lock(&theMonitor->mutex); //Lock the mutex to ensure that only one thread can access the shared resource.
        while (theMonitor->count < consumeSize) //A loop for when the consumer wants to remove units from the vending machine but there are not enough units in the vending machine so the consumer will wait until the vending  machine is filled enough to remove the wanted units.
        {
            logConsumerWait(name);   //Send a wait message to the log to print it out to the terminal.
            pthread_cond_wait(&theMonitor->consumer_cond, &theMonitor->mutex); //wait until the supplier signals that the consumer can keep trying to remove units.
            
        }

            theMonitor->consumed += consumeSize; //Adds the consume size of the consumer to the total consumed counter.
            theMonitor->count -= consumeSize; //Removes units from the total stock because the consumer removed units.
            logConsumerRemoved(name, consumeSize); //Send a message to the log that the consumer removed units.
            pthread_cond_broadcast(&theMonitor->supplier_cond); //Wake up all the waiting supplier threads.
            pthread_mutex_unlock(&theMonitor->mutex); //Unlock the lock so other threads can acces the shared resource.
            sleep(consumptionInterval); //Sleep some time before the next action as written in the txt file.
        
    }
    fprintf(stdout,"Consumer %s completed\n",name);
	return NULL;
}
/* Adds a log line that the consumer removed units from the machine
    Parameters:
    czConsumerName - the name of the consumer
    iConsumeSize - the number of units consumed
*/
void logConsumerRemoved(char* czConsumerName, int iConsumeSize)
{
    // Print a log line about consuming
    time_t current_time = time(NULL); //Using the time() function to get the current time and store it in a variable called current_time.
    char* time_string = ctime(&current_time); //Using the ctime() function to convert the value stored in current_time to a string representation of the local time.
    time_string = strtok(time_string, "\n"); //Gets rid of unwanted new lines.
    printf("%s %s removed %d units. Total consumed = %d. Stock after = %d\n",time_string ,czConsumerName, iConsumeSize, theMonitor->consumed, theMonitor->count);
    return;
}

/* Adds a log line that the consumer is going to wait
    Parameters:
    czConsumerName - the name of the consumer
*/
void logConsumerWait(char* czConsumerName)
{
    // Print a log line about going to wait
    time_t current_time = time(NULL);
    char* time_string = ctime(&current_time);
    time_string = strtok(time_string, "\n");
    printf("%s %s going to wait.\n",time_string , czConsumerName);
    return;
}

// 5783
