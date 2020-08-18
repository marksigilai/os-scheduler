#define _XOPEN_SOURCE
#define _XOPEN_SOURCE_EXTENDED
#include "scheduler.h"

#include <sys/time.h>
#include <assert.h>
#include <curses.h>
#include <ucontext.h>
#include <setjmp.h>
#include "util.h"
#include <stdio.h>
// This is an upper limit on the number of tasks we can create.
#define MAX_TASKS 128

// This is the size of each task's stack memory
#define STACK_SIZE 65536

// This struct will hold the all the necessary information for each task
typedef struct task_info {
  // This field stores all the state required to switch back to this task
  ucontext_t context;
  
  // This field stores another context. This one is only used when the task
  // is exiting.
  ucontext_t exit_context;
  
  // TODO: Add fields here so you can:
  //   a. Keep track of this task's state.
  //   b. If the task is sleeping, when should it wake up?
  //   c. If the task is waiting for another task, which task is it waiting for?
  //   d. Was the task blocked waiting for user input? Once you successfully
  //      read input, you will need to save it here so it can be returned.
  size_t wakeuptime;
  //current state
  int currentstate;
  //task being waited for
  int wait_task;
  int input;
} task_info_t;

int WAIT_INPUT = 6;
int READY = 5;
int EXIT_STATE = 4;
int SLEEP_STATE = 3;
int BLOCKED = 2;
int RUNNING = 1;


int current_task = 0; //< The handle of the currently-executing task
int num_tasks = 1;    //< The number of tasks created so far
task_info_t tasks[MAX_TASKS]; //< Information for every task


long long current_timestamp();
/**
 * Initialize the scheduler. Programs should call this before calling any other
 * functiosn in this file.
 */

void scheduler_init() {
  // TODO: Initialize the state of the scheduler
	tasks[current_task].currentstate = RUNNING;
	
	tasks[current_task].exit_context.uc_stack.ss_sp = malloc(STACK_SIZE);
	tasks[current_task].exit_context.uc_stack.ss_size = STACK_SIZE;
	
}
void schedule(){
  int i = current_task - 1;
  //printf("This is i %d", i);
  while(&tasks[(i++)] != NULL){
    i = (i)%num_tasks;
    
 //  printf("This is i %d the state for i %d\n", i, tasks[i].currentstate);
    if(tasks[i].currentstate == RUNNING){
      	//printf("THe running task is %d\n", i);
	    return;
    }
    else if(tasks[i].currentstate == BLOCKED){
        if(tasks[tasks[i].wait_task].currentstate == EXIT_STATE){
          tasks[i].currentstate = READY;
          //printf("WE got here");
        }
	}
    else if(tasks[i].currentstate == WAIT_INPUT){
	int input = getch();
	 if(input != ERR){
		//printw("The waiting task  is %d\n", i);
		tasks[i].currentstate = READY;
		tasks[i].input = input;
		return;	
	 }
    }
    else if(tasks[i].currentstate == SLEEP_STATE){	
	//printf(" %ld  ", tasks[i].wakeuptime);
	//printf(" %ld  \n", time_ms());
	if(time_ms() > tasks[i].wakeuptime){
        	//printw("Task %d is in the sleep state\n ", i);
		tasks[i].currentstate = READY;
	}  
    }
    else if(tasks[i].currentstate == READY){
      //printw("The ready task i %d is\n ", i);
	tasks[i].currentstate = RUNNING;
      int temp = current_task;
      current_task = i;
      swapcontext(&tasks[temp].context, &tasks[i].context);
    }
  }
}

/**
 * This function will execute when a task's function returns. This allows you
 * to update scheduler states and start another task. This function is run
 * because of how the contexts are set up in the task_create function.
 */
void task_exit() {
  // TODO: Handle the end of a task's execution here
  //printf(" the current exiting task is %d\n ", current_task);
  tasks[current_task].currentstate = EXIT_STATE;
	//tasks[tasks[current_task].wait_task].currentstate = READY;
  //printf(" the wait  task is %d\n ", tasks[current_task].wait_task);
  for(int k = 0; k < num_tasks; k++){
    if(tasks[k].wait_task == current_task){
      tasks[k].currentstate = READY;
      //printf("WE got here");
    }
  }
	schedule();
}

/**
 * Create a new task and add it to the scheduler.
 *
 * \param handle  The handle for this task will be written to this location.
 * \param fn      The new task will run this function.
 */
void task_create(task_t* handle, task_fn_t fn) {
  // Claim an index for the new task
  int index = num_tasks;
  num_tasks++;
  
  // Set the task handle to this index, since task_t is just an int
  *handle = index;
 
  // We're going to make two contexts: one to run the task, and one that runs at the end of the task so we can clean up. Start with the second
  
  // First, duplicate the current context as a starting point
  getcontext(&tasks[index].exit_context);
  
  // Set up a stack for the exit context
  tasks[index].exit_context.uc_stack.ss_sp = malloc(STACK_SIZE);
  tasks[index].exit_context.uc_stack.ss_size = STACK_SIZE;
  
  // Set up a context to run when the task function returns. This should call task_exit.
  makecontext(&tasks[index].exit_context, task_exit, 0);
  
  // Now we start with the task's actual running context
  getcontext(&tasks[index].context);
  
  // Allocate a stack for the new task and add it to the context
  tasks[index].context.uc_stack.ss_sp = malloc(STACK_SIZE);
  tasks[index].context.uc_stack.ss_size = STACK_SIZE;
  
  // Now set the uc_link field, which sets things up so our task will go to the exit context when the task function finishes
  tasks[index].context.uc_link = &tasks[index].exit_context;
  
  // And finally, set up the context to execute the task function
  makecontext(&tasks[index].context, fn, 0);

  //make the created task ready
  tasks[index].currentstate = READY;
  tasks[index].wait_task = -1;
}

/**
 * Wait for a task to finish. If the task has not yet finished, the scheduler should
 * suspend this task and wake it up later when the task specified by handle has exited.
 *
 * \param handle  This is the handle produced by task_create
 */
void task_wait(task_t handle) {
  // TODO: Block this task until the specified task has exited.
  // (*(&tasks[handle].currentstate) != EXIT_STATE){
	tasks[current_task].currentstate = BLOCKED;
	tasks[current_task].wait_task = handle;
	//current_task = handle;
	schedule();
  if(tasks[current_task].currentstate == EXIT_STATE){
    tasks[handle].currentstate = READY;
  }
  
}

/**
 * The currently-executing task should sleep for a specified time. If that time is larger
 * than zero, the scheduler should suspend this task and run a different task until at least
 * ms milliseconds have elapsed.
 * 
 * \param ms  The number of milliseconds the task should sleep.
 */
void task_sleep(size_t ms) {
  // TODO: Block this task until the requested time has elapsed.
  // Hint: Record the time the task should wake up instead of the time left for it to sleep. The bookkeeping is easier this way.
  long currtime = time_ms(); 
  tasks[current_task].currentstate = SLEEP_STATE;
  tasks[current_task].wakeuptime = currtime+ms;
  //sleep_ms(ms);
  schedule();
}

/**
 * Read a character from user input. If no input is available, the task should
 * block until input becomes available. The scheduler should run a different
 * task while this task is blocked.
 *
 * \returns The read character code
 */
int task_readchar() {
  // TODO: Block this task until there is input available.
  // To check for input, call getch(). If it returns ERR, no input was available.
  // Otherwise, getch() will returns the character code that was read.
//	char input;
	
	//printf("Current task %d", current_task);
	//scanf("%s", input);
 	//refresh();
	//input = getch();
	//if(getch() == ERR){
		//printf("The worf is %c", getch());
		tasks[current_task].currentstate = WAIT_INPUT;
		schedule();
	//}else{
		return tasks[current_task].input;
	//}
	
		
}

