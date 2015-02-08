Yanis Hattab
260535922
------------------

PA1 Thread Package
------------------

I wrote my code in myThreadPackage.c and made a header file to declare structures and signatures,
that header file is named myThreadPackage.h

To test my thread package, I wrote a version of the dining-philosophers problem that uses my
thread package by both creating threads and using semaphores to solve the problem for 5 philosophers.

My Makefile builds an executable file named diner that simulates the solution for 5 philosopers eating 5 times.
My Makefile also builds a file named myThreadPackage.o since there are no main methods in the 
package we can't build an executable out of it.




------------------

Methods signatures:

--> I have implemented all the required functionalities, this merely for indication.

int mythread_init();
int mythread_create(char *threadname, void (*threadfunc)(), int stacksize, int arg);
	For this I added an argument of type int called arg that will hold the argument 
	passed to the function, I needed it to test the diner.

void mythread_exit();
void runthreads();
void set_quantum_size(int quantum);
int create_semaphore(int value);
void semaphore_wait(int sem);
void semaphore_signal(int sem);
void destroy_semaphore(int sem);
void mythread_state();
void switcher();
	This is my round robin scheduler that is called each time SIGALRM happens, it will
	carry the context switch.

char* toString(threadState s);
