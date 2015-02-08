/*
Yanis Hattab
260535922
*/


#include <signal.h>
#include <slack/list.h>
#include <slack/std.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ucontext.h>
#include <string.h>
#include <sys/time.h>
#include "myThreadPackage.h"

#define THREAD_MAX_NUMBER 100
#define SEMAPHORE_MAX_NUMBER 100

int* semaphores; 

/*Fields declarations
*/
int nbThreads, semaphoreCount;

int executingThread;

List* runQueue;

mythread_control_block* threadControlTable;
semaphore* semaphoreTable;

//timer for the scheduler
struct itimerval tval;

//this variable will hold the current quantum in nanoseconds
int turnDuration;

//set of signals
sigset_t sset, oldset;


int mythread_init()
{
	nbThreads = semaphoreCount = 0;
	runQueue = list_create(NULL); 
	executingThread = 1;
	int i;
	/*THREAD CONTROL TABLE INITIALIZATION*/ 
	ucontext_t mainContext;
	threadControlTable = (mythread_control_block *) calloc(THREAD_MAX_NUMBER, sizeof(mythread_control_block)); 
	//I initialize all the thread IDs to -1 to show that they're empty
	for(i = 1; i < THREAD_MAX_NUMBER; i++) threadControlTable[i].thread_id = -1;

	/*SLOT ZERO IS NOT USED AND SERVES AS AN EMPTY RUNQUEUE FLAG
		This is necessary since an empty libstack queue returns 0 , we need to differentiate
	*/
	threadControlTable[0].thread_id = 0;

	//I use slot 0 to store the main thread
	getcontext(&mainContext);
	threadControlTable[1].thread_id = 1;
	threadControlTable[1].threadCPUTime = 0;
	threadControlTable[1].context = mainContext;
	threadControlTable[1].thread_name = "main";
	threadControlTable[1].state = RUNNING;
	nbThreads++;


	//SEMAPHORE TABLE INITIALIZATION
	semaphoreTable = (semaphore *) calloc(SEMAPHORE_MAX_NUMBER, sizeof(semaphore)); 
	for(i = 0; i < SEMAPHORE_MAX_NUMBER; i++) semaphoreTable[i].isUsed = 0;

	//TIMER INITIALIZATION
	tval.it_interval.tv_sec = 0;
	tval.it_interval.tv_usec = 100;
	tval.it_value.tv_sec = 0;
	tval.it_value.tv_usec = 100;
	//default quantum
	turnDuration = 100;
	

	//empty the set and add SIGALRM to it
	sigemptyset(&sset);
	sigaddset(&sset, SIGALRM);

	return 0;


}

int mythread_create(char *threadname, void (*threadfunc)(), int stacksize, int arg)
{

	ucontext_t context;

	//Look for an available spot in the thread block table, if its full createdThread will stay -1
	int i;
	int createdThread = -1;
	for(i = 2; i < THREAD_MAX_NUMBER; i++)
	{
		if(threadControlTable[i].thread_id == -1)
		{
			//create a thread block initialized to appropriate fields
			threadControlTable[i].thread_id = i;
			threadControlTable[i].threadCPUTime = 0;
			threadControlTable[i].thread_name = threadname;
			threadControlTable[i].state = RUNNABLE;
			createdThread = i;
			break;
		}
	}

	//Error if table is full
	if(createdThread < 0)
	{
		perror("Thread Package Error: Maximum number of threads reached.");
		return -1;
	}

	nbThreads++;
	//saves current context in context
	getcontext(&context);
	//adjust stack properties
	context.uc_stack.ss_sp = (char *) calloc(stacksize, sizeof(char));
	context.uc_stack.ss_size = stacksize;
	//make a context that starts at thread func
	makecontext(&context, threadfunc, 1, arg);
	threadControlTable[createdThread].context = context;

	//Add newly created thread to run queue
	runQueue = list_append_int(runQueue, createdThread);

	return createdThread;

}

void mythread_exit()
{
	//dequeue next thread to run
	int exitingThread = executingThread;
	executingThread = list_shift_int(runQueue);
	//adjust the states of the threads
	threadControlTable[executingThread].state = RUNNING;
	threadControlTable[exitingThread].state = EXIT;
	//update the CPU time
	threadControlTable[exitingThread].threadCPUTime += turnDuration;
	//Check if the runqueue is empty
	if(executingThread == 0)
	{
		perror("Thread Package Error: No more runnable threads.\n");
		exit(EXIT_FAILURE);
	}

	// mythread_control_block b = threadControlTable[executingThread];
	swapcontext(&threadControlTable[exitingThread].context, &threadControlTable[executingThread].context);

}

void runthreads()
{
	//set ALARM SIGNAL to trigger switcher fucntion
    int sigset();
    sigset(SIGALRM, switcher);
	setitimer(ITIMER_REAL, &tval, 0);

	//activates the Thread Switcher for the first time
	switcher();
}

void set_quantum_size(int quantum)
{
	//sets the quantum size of the round robin scheduler
	double q = ((double) quantum);

	turnDuration = (int)q;

	tval.it_interval.tv_sec = 0;
	tval.it_interval.tv_usec = q;
	tval.it_value.tv_sec = 0;
	tval.it_value.tv_usec = q;



}

int create_semaphore(int value)
{
	if(value < 0)
	{
		//value of semaphore connot be negative, we return error (-1)
		perror("Thread Package Error: Impossible to create a negative semaphore.\n");
		return -1;
	}
	else
	{
		int i;
		for(i = 0; i < SEMAPHORE_MAX_NUMBER; i++)
		{
			if(semaphoreTable[i].isUsed == 0)
			{
				//create a semaphore initialized to value
				semaphoreTable[i].isUsed = 1;
				semaphoreTable[i].count = value;
				semaphoreTable[i].initialCount = value;
				semaphoreTable[i].waitQueue = list_create(NULL);
				semaphoreCount++;
				return i;
			}
		}
	}
	
	//this error return means no room for a semaphore
	return -1;

}

void semaphore_wait(int sem)
{
	if(semaphoreTable[sem].isUsed == 0 || sem >= SEMAPHORE_MAX_NUMBER || sem < 0) 
	{
		perror("Thread Package Error: Not a valid semaphore.\n");
		return;
	}
	else
	{
		//block signals
		sigprocmask(SIG_BLOCK, &sset, &oldset);

		semaphoreTable[sem].count--;
		if(semaphoreTable[sem].count < 0)
		{
			semaphoreTable[sem].waitQueue = list_append_int(semaphoreTable[sem].waitQueue, executingThread);
			//update thread state
			threadControlTable[executingThread].state = WAIT;

			

			//swap context
			int waitingThread = executingThread;
			executingThread = list_shift_int(runQueue);
			threadControlTable[executingThread].state = RUNNING;


			sigprocmask(SIG_SETMASK, &oldset, 0);

			threadControlTable[waitingThread].threadCPUTime += turnDuration;

			//Check if the runqueue is empty
			if(executingThread == 0)
			{
				perror("Thread Package Error: No more runnable threads.\n");
				exit(EXIT_FAILURE);
			}
			
			swapcontext(&threadControlTable[waitingThread].context, &threadControlTable[executingThread].context);
		}
		else sigprocmask(SIG_SETMASK, &oldset, 0);

	}


}

void semaphore_signal(int sem)
{
	if(semaphoreTable[sem].isUsed == 0 || sem >= SEMAPHORE_MAX_NUMBER || sem < 0)
	{
		perror("Thread Package Error: Not a valid semaphore.\n");
		return;
	}


	sigprocmask(SIG_BLOCK, &sset, &oldset);

	if(semaphoreTable[sem].count < 0)
	{
		semaphoreTable[sem].count++;
		//some thread is waiting, we wake it and enqueue it in runqueue
		int resumingThread = list_shift_int(semaphoreTable[sem].waitQueue);
		semaphoreTable[sem].waitQueue = list_append_int(runQueue, resumingThread);
		// update thread state
		threadControlTable[resumingThread].state = RUNNABLE;
	}
	else semaphoreTable[sem].count++;

	sigprocmask(SIG_SETMASK, &oldset, 0);
}


void destroy_semaphore(int sem)
{
	sigprocmask(SIG_BLOCK, &sset, &oldset);
	//check if there are waiting threads
	if(semaphoreTable[sem].count < 0)
	{
		perror("Thread Package Error: Semaphore can't be deleted because threads are waiting on it.\n");
	}
	else
	{
		//check if semaphore count changed from it's initial value
		if(semaphoreTable[sem].count != semaphoreTable[sem].initialCount) 
		{
			perror("Thread Package Warning: The value of this semaphore has changed.\n");
		}
		//blank the fields
		semaphoreTable[sem].isUsed = 0;
		semaphoreTable[sem].count = 0;
		semaphoreTable[sem].initialCount = 0;
		free(semaphoreTable[sem].waitQueue);
		semaphoreTable[sem].waitQueue = list_create(NULL);
		semaphoreCount--;
	}

	sigprocmask(SIG_SETMASK, &oldset, 0);
}

void mythread_state()
{
	//Iterates through all thread in thread control table and prints their properties
	printf("\nCurrent Threads:\n");
	int i;
	for(i = 1; i < THREAD_MAX_NUMBER; i++)
	{
		if(threadControlTable[i].thread_id != -1)
		{
			//cast its state to a string
			char* s = toString(threadControlTable[i].state);
			printf("%d)  %s  [%s]  %lu microseconds\n",threadControlTable[i].thread_id, threadControlTable[i].thread_name, s , threadControlTable[i].threadCPUTime);

		}
	}

	printf("Total: %d thread(s) and %d semaphores.\n\n", nbThreads, semaphoreCount);

}

void switcher()
{
	int currentThread = executingThread;
	runQueue = list_append_int(runQueue, currentThread);
	executingThread = list_shift_int(runQueue);
	//adjust threads states
	threadControlTable[currentThread].state = RUNNABLE;
	threadControlTable[executingThread].state = RUNNING;
	
	//compute & update CPU time
	threadControlTable[currentThread].threadCPUTime += turnDuration;

	//Check if the runqueue is empty
	if(executingThread == 0)
	{
		perror("Thread Package Error: No more runnable threads.\n");
		exit(EXIT_FAILURE);
	}

	//swap context
	swapcontext(&threadControlTable[currentThread].context, &threadControlTable[executingThread].context);

}

char* toString(threadState s)
{
	//function that returns string equivalent of a state
	switch(s)
	{
		case RUNNABLE : return "Runnable";
		case RUNNING  : return "Running";
		case WAIT  : return "Waiting";
		case BLOCKED  : return "Blocked";
		case EXIT : return "Exited";
		default : return NULL;
	}

}

