/*
Yanis Hattab
260535922
*/

#include <ucontext.h>
#include <slack/list.h>
#include <slack/std.h>

typedef enum {RUNNABLE, RUNNING, WAIT, BLOCKED, EXIT} threadState;

typedef struct
{
	ucontext_t context;
	char *thread_name;
	int thread_id;
	long threadCPUTime;
	threadState state;

}mythread_control_block;

typedef struct
{
	int count;
	int initialCount;
	List *waitQueue;
	int isUsed;

}semaphore;


int mythread_init();
int mythread_create(char *threadname, void (*threadfunc)(), int stacksize, int arg);
void mythread_exit();
void runthreads();
void set_quantum_size(int quantum);
int create_semaphore(int value);
void semaphore_wait(int sem);
void semaphore_signal(int sem);
void destroy_semaphore(int sem);
void mythread_state();
void switcher();
char* toString(threadState s);



