/*
 * In this example I show communication via signals (IPC).
 * The father task registers an ASR, creates a child and the child sends three different signals (2 equal).
 * They are left pending until
 * the father task is scheduled. So father task receives both signals (one discarded). This makes the father
 * invalidate all the signals. Child is scheduled and another signal sent. Father is scheduled and does
 * not receive any signal. It terminates.
 */

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <rtems.h>

rtems_id fatherid, childid;

rtems_task task1_entrypoint(rtems_task_argument ignored)
{
	rtems_status_code temp[3];
	rtems_task_priority p;

//Sending signals
	temp[0] = rtems_signal_send(fatherid, RTEMS_SIGNAL_5);
	temp[1] = rtems_signal_send(fatherid, RTEMS_SIGNAL_4);
	temp[2] = rtems_signal_send(fatherid, RTEMS_SIGNAL_5);
	assert(temp[0] == RTEMS_SUCCESSFUL);
	assert(temp[1] == RTEMS_SUCCESSFUL);
	assert(temp[2] == RTEMS_SUCCESSFUL);
	printf("Child: signals 5, 4, 5 sent to father\n");


//Scheduling father (priority change because otherwise child would be scheduled). GOTO 1
	temp[0] = rtems_task_set_priority(rtems_task_self(), 10, &p);
	assert(temp[0] == RTEMS_SUCCESSFUL);
	rtems_task_wake_after(RTEMS_YIELD_PROCESSOR);


//GOTO 2
//Sending another signal and scheduling father
	temp[0] = rtems_signal_send(fatherid, RTEMS_SIGNAL_2);
	assert(temp[0] == RTEMS_NOT_DEFINED); //at this point, father has invalidated the ASR
	printf("Child: signal sent to father\n");
	temp[0] = rtems_task_set_priority(rtems_task_self(), 10, &p);
	assert(temp[0] == RTEMS_SUCCESSFUL);
	rtems_task_wake_after(RTEMS_YIELD_PROCESSOR); //GOTO 3
}

//GOTO 1
void user_routine(rtems_signal_set signals)
{
	rtems_status_code temp;
	rtems_task_priority oldpriority;

//Invalidating all signals
	printf("\tFather has just received signal set: %ld\n", signals);
	printf("\tRTEMS_SIGNAL_1: %lu\n", signals & RTEMS_SIGNAL_1);
	printf("\tRTEMS_SIGNAL_2: %lu\n", signals & RTEMS_SIGNAL_2);
	printf("\tRTEMS_SIGNAL_3: %lu\n", signals & RTEMS_SIGNAL_3);
	printf("\tRTEMS_SIGNAL_4: %lu\n", signals & RTEMS_SIGNAL_4);
	printf("\tRTEMS_SIGNAL_5: %lu\n", signals & RTEMS_SIGNAL_5);
	printf("\tRTEMS_SIGNAL_6: %lu\n", signals & RTEMS_SIGNAL_6);
	printf("\tRTEMS_SIGNAL_7: %lu\n", signals & RTEMS_SIGNAL_7);
	printf("\tRTEMS_SIGNAL_8: %lu\n", signals & RTEMS_SIGNAL_8);
	printf("\tRTEMS_SIGNAL_9: %lu\n", signals & RTEMS_SIGNAL_9);
	printf("\tRTEMS_SIGNAL_10: %lu\n", signals & RTEMS_SIGNAL_10);
	printf("\t...\n");

	assert(RTEMS_SUCCESSFUL == rtems_signal_catch(NULL, RTEMS_DEFAULT_MODES));
	printf("ASR invalidated\n");


//Scheduling child
	temp = rtems_task_set_priority(childid, 3, &oldpriority);
	assert(temp == RTEMS_SUCCESSFUL);
	rtems_task_wake_after(RTEMS_YIELD_PROCESSOR); //GOTO 2
}

//Execution flow begins here. This is referred to as the application
rtems_task Init(rtems_task_argument argument)
{
	rtems_status_code status, temp;
	rtems_task_priority priority = 3;
	rtems_name name = rtems_build_name('C', 'T', '1', ' ');

//Getting father's ID
	fatherid = rtems_task_self();
	printf("Father task has ID: %ld\n", fatherid);


//Installing ASR
	temp = rtems_signal_catch(user_routine, RTEMS_DEFAULT_MODES);
	assert(temp == RTEMS_SUCCESSFUL);


//Creating 1 child
	// prototype: rtems_task_create( name, initial_priority, stack_size, initial_modes, attribute_set, *id );
	status = rtems_task_create(name, priority, RTEMS_MINIMUM_STACK_SIZE * 2,
		RTEMS_DEFAULT_MODES, RTEMS_DEFAULT_ATTRIBUTES, &childid);
	assert(status == RTEMS_SUCCESSFUL);
	printf("1 task created successfully (id: %ld), but it is dormant\n", childid);


	// prototype: rtems_task_start(id, entry_point, argument)
	status = rtems_task_start(childid, task1_entrypoint, 0); //Notice: at this point, this task is not pre-empted because of the default configuration
	assert(status == RTEMS_SUCCESSFUL);
	printf("1 task started successfully and it is ready\n");


//Scheduling child
	rtems_task_wake_after(RTEMS_YIELD_PROCESSOR);
	//GOTO 3
	printf("Father: finishing...\n");
}

#include <bsp.h> /* for device driver prototypes */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS             4

#define CONFIGURE_EXTRA_TASK_STACKS         (6 * RTEMS_MINIMUM_STACK_SIZE)

#define CONFIGURE_MICROSECONDS_PER_TICK     1000 /* Each 1 millisecond a tick */

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE //So default entry point is Init
#define CONFIGURE_INIT_TASK_PRIORITY 4
//#define CONFIGURE_INIT_TASK_INITIAL_MODES RTEMS_PREEMPT

#define CONFIGURE_INIT
#include <rtems/confdefs.h>

/* end of file */
