/*
Yanis Hattab
260535922
*/

#include <slack/std.h>
#include <slack/list.h>
#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>
#include <string.h>
#include <signal.h>
#include <sys/time.h>
#include <time.h>
#include "myThreadPackage.h"

#define DELAY 1000000

//Semaphores used by philosophers
int* semaphores; 

/*
    Test case built around the dinning philosophers problem and solving it with semaphores
*/

void philosopher(int i)
{   
    //Each philosopher will eat 5 times before exiting
	int j;
    int k = 0;
	do{
		semaphore_wait(semaphores[i]);
		semaphore_wait(semaphores[(i + 1) % 5]);

		//Philosopher eats
		j = 0;
		printf("Philosopher %d starts to eat.\n", i);
		while ( j++ < DELAY);
		printf("Philosopher %d has finished eating.\n", i);

		semaphore_signal(semaphores[i]);
		semaphore_signal(semaphores[(i + 1) % 5]);

		//Philosopher thinks
		j = 0;
		printf("Philosopher %d starts to think.\n", i);
		while ( j++ < DELAY);
		printf("Philosopher %d has finished thinking.\n", i);

	} while(k++ < 5);

    printf("Philosopher %d leaves the diner.\n", i);
    mythread_exit();
}


int main()
{
	printf("\nTESTING DINING PHILOSOPHERS!\n");
    mythread_init();

    semaphores = (int*) calloc(5, sizeof(int));

    semaphores[0] = create_semaphore(1);
    semaphores[1] = create_semaphore(1);
    semaphores[2] = create_semaphore(1);
    semaphores[3] = create_semaphore(1);
    semaphores[4] = create_semaphore(1);

    mythread_create("Socrates (1)", &philosopher, 16384, 0);
    mythread_create("Plato (2)", &philosopher, 16384, 1);
    mythread_create("Descartes (3)", &philosopher, 16384, 2);
    mythread_create("Hegel (4)", &philosopher, 16384, 3);
    mythread_create("Camus (5)", &philosopher, 16384, 4);
     

    set_quantum_size(1000);
    printf("Let them think!\n");
    mythread_state();
    runthreads();

    
    //this makes the main method idle for a some time to let threads run
    int l = 0;
    while(l++ < 100000000);
    printf("The dinner is done\n");
    mythread_state();
    printf("Destroying semaphores\n");
    destroy_semaphore(semaphores[0]);
    destroy_semaphore(semaphores[1]);
    destroy_semaphore(semaphores[2]);
    destroy_semaphore(semaphores[3]);
    destroy_semaphore(semaphores[4]);
    printf("Finished.\n");
    return 0;
}

