/*
	Yanis Hattab
	
	Printer buffer usage simulation
	
	Producer/Consumer problem solved using threads and semaphores

*/
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>

//Program duration and interval between requests from clients as constants
#define PROGRAM_DURATION 60
#define CLIENT_REQUEST_INTERVAL 5

sem_t mutex;
sem_t bufferHasSpace;
sem_t bufferHasJob;

int nbClients;
int nbPrinters;
int buffSize;


/*
*	Buffer implementation as a circular array (and its helper functions)
*/
typedef struct 
{
	int size;
	int start;
	int end;
	int count;
	int *members;
}CircularBuffer;

void cbInit(CircularBuffer *cb, int aSize)
{
	(*cb).size = aSize;
	(*cb).start = 0;
	(*cb).end = 0;
	(*cb).count = 0;
	(*cb).members = calloc(aSize, sizeof(int));
}

int cbIsFull(CircularBuffer *cb)
{
	return (*cb).count==(*cb).size;
}

int cbIsEmpty(CircularBuffer *cb)
{
	return (*cb).count == 0;
}

void cbPush(CircularBuffer *cb, int element)
{
	(*cb).members[(*cb).end] = element;
	(*cb).end = ((*cb).end + 1) % (*cb).size;
	(*cb).count++;
	return;
}

int cbPull(CircularBuffer *cb)
{
	int toReturn = (*cb).members[(*cb).start];
	(*cb).start = ((*cb).start + 1) % (*cb).size;
	(*cb).count--;
	return toReturn;
}
/*
*		End of Buffer implementation
*/


//Buffer declaration
CircularBuffer spooler;


void * client(void *clientId)
{
	int clientNb = (intptr_t)clientId;

	/*
	*	A client will keep sending requests after sleeping for an interval
	*/
	while(1)
	{
		int fullFlag = 0; // if buffer full this flag is set so that the client sleeps
		int nbPages = (rand() % 10) + 1;
		
		//checking buffer to see if it's full
		fullFlag = cbIsFull(&spooler);

		//If full we wait else we add the job to the spooler
		if(fullFlag)
		{
			printf("Client %d has %d pages to print, buffer full, sleeps\n", 
					clientNb, nbPages);
			sem_wait(&bufferHasSpace); //wait till buffer has an available spot 
			printf("Client %d wakes up, puts request in Buffer[%d]\n", 
				clientNb, spooler.end);
		}
		else
		{
			printf("Client %d has %d pages to print, puts request in Buffer[%d]\n", 
				clientNb, nbPages, spooler.end);
			sem_wait(&bufferHasSpace);
		}

		//Critical section to add something to the buffer
		sem_wait(&mutex);
		cbPush(&spooler, nbPages);
		sem_post(&mutex);
		sem_post(&bufferHasJob);
		//end of buffer access

		sleep(CLIENT_REQUEST_INTERVAL); // Interval a client waits before sending a new request, CAN BE CHANGED
	}
}


void * printer(void *printerId)
{
	/*
	*	A printer will keep servicing jobs in buffer unless it's empty then it will sleep
	*	until a new job appears
	*/
	
	int printerNb = (intptr_t)printerId;
	
	while(1)
	{
		int emptyFlag = 0; //set to 1 if the buffer is empty
		int pages; //store pages its printing
		int index;	//store buffer index servicing

		//checking if buffer is empty
		emptyFlag = cbIsEmpty(&spooler);

		if(emptyFlag)
		{
			//If no jobs in the buffer, the printer will sleep
			printf("No requests in buffer, Printer %d sleeps\n", printerNb);
		}

		//critical section of retrieving a job
		sem_wait(&bufferHasJob); //this will make the printer sleep if there are no jobs
		sem_wait(&mutex);
		index = spooler.start;
		pages = cbPull(&spooler);
		sem_post(&mutex);
		sem_post(&bufferHasSpace);	
		//end of critical section

		//priniting simulation
		printf("Printer %d starts printing %d pages from Buffer[%d]\n", 
					printerNb, pages, index);
		sleep(pages);
		printf("Printer %d finishes printing %d pages from Buffer[%d]\n", 
					printerNb, pages, index);
	}		
}


int main()
{
	//Get parameters from user
	printf("Enter the number of clients:\n");
	scanf("%d", &nbClients);
	printf("Enter the number of printers:\n");
	scanf("%d", &nbPrinters);
	printf("Enter the buffer size:\n");
	scanf("%d", &buffSize);

	//initializing buffer and semaphores
	cbInit(&spooler, buffSize);
	sem_init(&mutex, 0, 1);	//mutual exclusion lock for accessing the buffer
	sem_init(&bufferHasJob, 0, 0); //starts as underflow until a job is added
	sem_init(&bufferHasSpace, 0, buffSize); //when it reaches 0 -> buffer overflow

	//initializing threads pointers and helper variables
	int i;
	int rc;
	pthread_t * clientThreads = calloc(nbClients, sizeof(pthread_t));
	pthread_t * printerThreads = calloc(nbPrinters, sizeof(pthread_t));
	srand(time(NULL)); //seed for random int generator

	//creating printers as threads
	for(i=0; i<nbPrinters; i++)
	{
		rc = pthread_create(&printerThreads[i], NULL, *printer, (void *) (intptr_t) i);
		if(rc){
			printf("Error while creating threads\n");
			exit(-1);
		}
	}

	//creating clients as threads
	for(i=0; i<nbClients; i++)
	{
		rc = pthread_create(&clientThreads[i], NULL, *client, (void *) (intptr_t)i);
		if(rc){
			printf("Error while creating threads\n");
			exit(-1);
		}
	}

	sleep(PROGRAM_DURATION); //The program runs for one minute, THIS CAN BE CHANGED
	printf("Finished\n");
	return 0;
}