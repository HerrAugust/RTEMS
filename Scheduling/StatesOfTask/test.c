/*
 * Here we want to show how to alter the automatically calculated order of tasks.
 * First, the father task creates two children with a certain priority. Then, is assigns another priority to one child
 * resulting in change of the order of tasks. We show a first case in which these two processes are scheduled
 * with priorities resp. 2 and 3 and then 3 and 2.
 */

//#include <bsp.h>

#include <stdio.h>
#include <stdlib.h>

rtems_task task1_entrypoint(rtems_task_argument ignored)
{
	rtems_id id = rtems_task_self();
	rtems_task_priority priority;
	rtems_task_set_priority(id, RTEMS_CURRENT_PRIORITY, &priority); //returns current task priority
	printf("\tThis is the task with id %ld and with priority %ld\n", id, priority);
}

rtems_task task2_entrypoint(rtems_task_argument ignored)
{
	rtems_id id = rtems_task_self();
	rtems_task_priority priority;
	rtems_task_set_priority(id, RTEMS_CURRENT_PRIORITY, &priority); //returns current task priority
	printf("\tThis is the task with id %ld and with priority %ld\n", id, priority);
}




void *POSIX_Init(void *argument)
{
	rtems_status_code status[2];
	rtems_id ids[2], myid;
	rtems_task_priority priorities[2] = { 2, 3 };//VS { 5, 6 }; : father task has highest priority and so no child is
	rtems_name names[2];
	rtems_task_priority p;
	rtems_mode oldmode;

	names[0] = rtems_build_name('C', 'T', '1', ' ');
	names[1] = rtems_build_name('C', 'T', '2', ' ');

//Getting father's properties
	//Getting father's ID
	myid = rtems_task_self();
	printf("Father task has ID: %ld\n", myid);

	//Changin father's priority	
	rtems_status_code temp = rtems_task_set_priority(myid, 4, &p); //otherwise children could not be scheduled
	assert(temp == RTEMS_SUCCESSFUL);
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
	status[0] = rtems_task_start(ids[0], task1_entrypoint, priorities[0]); //Notice: at this point, this task is pre-empted and control goes to function task1_entrypoint(), defined in task.c
	//printf("qui1\n");
	status[1] = rtems_task_start(ids[1], task2_entrypoint, priorities[1]);
	//printf("qui2\n");
	assert(status[0] == RTEMS_SUCCESSFUL);
	assert(status[1] == RTEMS_SUCCESSFUL);
	printf("2 tasks started successfully and they are ready\n");


//scheduling those two tasks
	rtems_task_wake_after(RTEMS_YIELD_PROCESSOR);




//END OF FIRST PART
	printf("\n\nEND OF FIRST PART\n\n")
//Creating two children with a change of priorities.
	status[0] = rtems_task_create(names[0], priorities[0], RTEMS_MINIMUM_STACK_SIZE * 2,
		RTEMS_DEFAULT_MODES, RTEMS_DEFAULT_ATTRIBUTES, &ids[0]);
	status[1] = rtems_task_create(names[1], priorities[1], RTEMS_MINIMUM_STACK_SIZE * 2,
		RTEMS_DEFAULT_MODES, RTEMS_DEFAULT_ATTRIBUTES, &ids[1]);
	assert(status[0] == RTEMS_SUCCESSFUL);
	assert(status[1] == RTEMS_SUCCESSFUL);
	printf("2 tasks created successfully (id: %ld and %ld), but they are dormant\n", ids[0], ids[1]);


	// prototype: rtems_task_start(id, entry_point, argument)
	status[0] = rtems_task_start(ids[0], task1_entrypoint, priorities[0]); //Notice: at this point, this task is pre-empted and control goes to function task1_entrypoint(), defined in task.c
	//printf("qui1\n");
	status[1] = rtems_task_start(ids[1], task2_entrypoint, priorities[1]);
	//printf("qui2\n");
	assert(status[0] == RTEMS_SUCCESSFUL);
	assert(status[1] == RTEMS_SUCCESSFUL);
	printf("2 tasks started successfully and they are ready\n");


//switching priorities
	status[0] = rtems_task_set_priority(ids[0], priorities[1], NULL);
	status[1] = rtems_task_set_priority(ids[1], priorities[0], NULL);
	assert(status[0] == RTEMS_SUCCESSFUL);
	assert(status[1] == RTEMS_SUCCESSFUL);


//scheduling those two tasks
	rtems_task_wake_after(RTEMS_YIELD_PROCESSOR);
}

/* configuration information */

#include <bsp.h> /* for device driver prototypes */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MICROSECONDS_PER_TICK     500   // NB: 10 and lower gives system failure for erc32 simulator

#define CONFIGURE_MAXIMUM_TASKS             4

#define CONFIGURE_EXTRA_TASK_STACKS         (6 * RTEMS_MINIMUM_STACK_SIZE)

// Needed for RM Mangager
#define CONFIGURE_MAXIMUM_PERIODS           1

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#include <rtems/confdefs.h>

/* end of file */
