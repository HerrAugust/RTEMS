/*
 * In this example I show communication via signals (IPC).
 * The father task creates a child and the child sends three different signals (2 equal). It is left pending until
 * the father task registers an ASR. So father task receives both signals (one discarded). This makes the father
 * invalidate all the signals. Child is scheduled and signal sent. Father is scheduled and does
 * not receive any signal. It terminates.
 */

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <rtems.h>


rtems_task task1_entrypoint(rtems_task_argument ignored)
{
	rtems_status_code temp;
	
}

//Execution flow begins here. This is referred to as the application
rtems_task Init(rtems_task_argument argument)
{
	rtems_status_code status[2], temp;
	rtems_id ids[2], myid;
	rtems_task_priority priorities[2] = { 2, 3 };
	rtems_name names[2];
	rtems_task_priority p;

	names[0] = rtems_build_name('C', 'T', '1', ' ');
	names[1] = rtems_build_name('C', 'T', '2', ' ');

//Getting father's properties
	//Getting father's ID
	myid = rtems_task_self();
	printf("Father task has ID: %ld\n", myid);

	//Changin father's priority	
	temp = rtems_task_set_priority(RTEMS_SELF, RTEMS_CURRENT_PRIORITY, &p); //returns current task priority
	assert(temp == RTEMS_SUCCESSFUL);
	printf("Father task has priority: %ld\n", p);


//creating 2 children
	// prototype: rtems_task_create( name, initial_priority, stack_size, initial_modes, attribute_set, *id );
	status[0] = rtems_task_create(names[0], priorities[0], RTEMS_MINIMUM_STACK_SIZE * 2,
		RTEMS_DEFAULT_MODES, RTEMS_DEFAULT_ATTRIBUTES, &ids[0]);
	assert(status[0] == RTEMS_SUCCESSFUL);
	printf("2 tasks created successfully (id: %ld and %ld), but they are dormant\n", ids[0], ids[1]);


	// prototype: rtems_task_start(id, entry_point, argument)
	status[0] = rtems_task_start(ids[0], task1_entrypoint, priorities[0]); //Notice: at this point, this task is not pre-empted because of the default configuration
	assert(status[0] == RTEMS_SUCCESSFUL);
	assert(status[1] == RTEMS_SUCCESSFUL);
	printf("2 tasks started successfully and they are ready\n");
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
