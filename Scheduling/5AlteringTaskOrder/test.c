/*
 * Here we want to show how to alter the automatically calculated order of tasks.
 * First, the father task creates two children with a certain priority. Then, is assigns another priority to one child
 * resulting in change of the order of tasks. We show a first case in which these two processes are scheduled
 * with priorities resp. 2 and 3 and then 3 and 2.
 */

//#include <bsp.h>

#include <stdio.h>
#include <stdlib.h>
#include <rtems.h>
#include <assert.h>

rtems_task task_entrypoint(rtems_task_argument ignored)
{
	rtems_id id = rtems_task_self();
	rtems_task_priority priority;
	rtems_task_set_priority(id, RTEMS_CURRENT_PRIORITY, &priority); //returns current task priority
	printf("\tThis is the task with id %ld and with priority %ld\n", id, priority);

	//Without deleting this task, it will never return to father
	if(rtems_task_delete(id) != RTEMS_SUCCESSFUL) {
		printf("\tCannot delete task %ld", id);
	}
}




rtems_task Init(rtems_task_argument ignored)
{
	rtems_status_code status[2], temp;
	rtems_id ids[2], myid;
	rtems_task_priority priorities[2] = { 2, 3 };//VS { 5, 6 }; : father task has highest priority and so no child is
	rtems_name names[2];
	rtems_task_priority p;

	names[0] = rtems_build_name('C', 'T', '1', ' ');
	names[1] = rtems_build_name('C', 'T', '2', ' ');

//Getting father's properties (because of a macro config. in commonheader.h, this is the first task executed and it is not pre-emptible and with priority 1)
	//Getting father's ID
	myid = rtems_task_self();
	printf("Father task has ID: %ld\n", myid);

	//Getting father's priority	(it is 4 because of CONFIGURE_INIT_TASK_PRIORITY)
	temp = rtems_task_set_priority(RTEMS_SELF, RTEMS_CURRENT_PRIORITY, &p); //returns current task priority
	assert(temp == RTEMS_SUCCESSFUL);
	printf("Father task has priority: %ld\n", p);


//creating 2 children
	// prototype: rtems_task_create( name, initial_priority, stack_size, initial_modes, attribute_set, *id );
	status[0] = rtems_task_create(names[0], priorities[0], RTEMS_MINIMUM_STACK_SIZE * 2,
		RTEMS_DEFAULT_MODES, RTEMS_DEFAULT_ATTRIBUTES, &ids[0]);
	status[1] = rtems_task_create(names[1], priorities[1], RTEMS_MINIMUM_STACK_SIZE * 2,
		RTEMS_DEFAULT_MODES, RTEMS_DEFAULT_ATTRIBUTES, &ids[1]);
	assert(status[0] == RTEMS_SUCCESSFUL);
	assert(status[1] == RTEMS_SUCCESSFUL);
	printf("2 tasks created successfully (id: %ld and %ld), but they are dormant\n", ids[0], ids[1]);


	// prototype: rtems_task_start(id, entry_point, argument)
	status[0] = rtems_task_start(ids[0], task_entrypoint, priorities[0]); //Notice: at this point, this task is pre-empted (because of the configuration below) and control goes to function task1_entrypoint()
	printf("here 1\n");
	status[1] = rtems_task_start(ids[1], task_entrypoint, priorities[1]);
	printf("here 2\n");
	assert(status[0] == RTEMS_SUCCESSFUL);
	assert(status[1] == RTEMS_SUCCESSFUL);
	printf("2 tasks started successfully and they are ready\n");


//scheduling those two tasks
	rtems_task_wake_after(RTEMS_YIELD_PROCESSOR);
	rtems_task_wake_after(RTEMS_YIELD_PROCESSOR);





//END OF FIRST PART
	printf("\n\nEND OF FIRST PART\n\n\n");
//Creating two children with a change of priorities.
	status[0] = rtems_task_create(names[0], priorities[0], RTEMS_MINIMUM_STACK_SIZE * 2,
		RTEMS_DEFAULT_MODES, RTEMS_DEFAULT_ATTRIBUTES, &ids[0]);
	status[1] = rtems_task_create(names[1], priorities[1], RTEMS_MINIMUM_STACK_SIZE * 2,
		RTEMS_DEFAULT_MODES, RTEMS_DEFAULT_ATTRIBUTES, &ids[1]);
	assert(status[0] == RTEMS_SUCCESSFUL);
	assert(status[1] == RTEMS_SUCCESSFUL);
	printf("2 tasks created successfully (id: %ld and %ld), but they are dormant\n", ids[0], ids[1]);


	// prototype: rtems_task_start(id, entry_point, argument)
	status[0] = rtems_task_start(ids[0], task_entrypoint, priorities[0]); //Notice: at this point, this task is pre-empted and control goes to function task1_entrypoint(), defined in task.c
	//printf("qui1\n");
	status[1] = rtems_task_start(ids[1], task_entrypoint, priorities[1]);
	//printf("qui2\n");
	assert(status[0] == RTEMS_SUCCESSFUL);
	assert(status[1] == RTEMS_SUCCESSFUL);
	printf("2 tasks started successfully and they are ready\n");


//switching priorities
	status[0] = rtems_task_set_priority(ids[0], priorities[0] + 6, &p);
	status[1] = rtems_task_set_priority(ids[1], priorities[1] + 3, &p);
	assert(status[0] == RTEMS_SUCCESSFUL);
	assert(status[1] == RTEMS_SUCCESSFUL);


//scheduling those two tasks
	rtems_task_wake_after(RTEMS_YIELD_PROCESSOR);
	rtems_task_wake_after(RTEMS_YIELD_PROCESSOR);
}

/* configuration information */

#include <bsp.h> /* for device driver prototypes */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MICROSECONDS_PER_TICK     1000 /* Each 1 millisecond a tick */
#define CONFIGURE_TICKS_PER_TIMESLICE		50   /* Each 50 milliseconds a context switch */

#define CONFIGURE_MAXIMUM_TASKS             4

#define CONFIGURE_EXTRA_TASK_STACKS         (6 * RTEMS_MINIMUM_STACK_SIZE)

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE //So default entry point is Init
#define CONFIGURE_INIT_TASK_PRIORITY 4
/*
#define CONFIGURE_INIT_TASK_INITIAL_MODES RTEMS_PREEMPT
*/

#define CONFIGURE_INIT
#include <rtems/confdefs.h>

/* end of file */
