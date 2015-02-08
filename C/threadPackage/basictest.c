#include <slack/std.h>
#include <slack/list.h>
#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>
#include <string.h>
#include <signal.h>
#include <sys/time.h>
#include <time.h>
#include "util.h"




void test()
{
	//semaphore_wait(sem1);

	printf("beginning %d\n", threadControlTable[executingThread].thread_id);
	int i = 0;
	while(i<10000000) i++;
	printf("%s (id %d) reached 10000\n", threadControlTable[executingThread].thread_name, threadControlTable[executingThread].thread_id);
	while(i<20000000) i++;
	printf("%s (id %d) reached 20000\n", threadControlTable[executingThread].thread_name, threadControlTable[executingThread].thread_id);

	
	//semaphore_signal(sem1);
	printf("signaled 1\n");
	mythread_exit();
}

void test2()
{
	//semaphore_wait(sem2);

	printf("beginning %d\n", threadControlTable[executingThread].thread_id);
	int i = 0;
	while(i<10000) i++;
	printf("%s (id %d) reached 10000\n", threadControlTable[executingThread].thread_name, threadControlTable[executingThread].thread_id);
	while(i<20000) i++;
	printf("%s (id %d) reached 20000\n", threadControlTable[executingThread].thread_name, threadControlTable[executingThread].thread_id);
	semaphore_signal(sem2);
	printf("signaled 2\n");
	mythread_exit();
}


 int main()
 {
     printf("TESTING!\n");
     mythread_init();


     sem1 = create_semaphore(0);
     sem2 = create_semaphore(0);

     mythread_create("testA", &test, 16384, 0);
     mythread_state();
     mythread_create("testB", &test2, 16384, 0);
 	 set_quantum_size(1000);
     runthreads();

     semaphore_wait(sem1);
     semaphore_wait(sem2);
     
     mythread_state();
     printf("DONE TESTING!\n");
     mythread_state();
     //while(1);
     return 0;
 }