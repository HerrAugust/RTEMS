/*
 * The application starts 2 tasks.
 * Tasks must print "I am task x and I am running" according to their priorities.
 * Expected behavior: 2 children tasks are created. One with priority 2 and the other 3.
 * 		child task with priority 3 is scheduled and then the one with priority 2, resulting in a certain order of printf.
 */

// Note: RTEMS_DEAULT_MODES = RTEMS_PREEMPT | RTEMS_NO_TIMESLICE | RTEMS_ASR | RTEMS_INTERRUPT_LEVEL(0)
// Note: RTEMS_DEFAULT_ATTRIBUTES = RTEMS_NO_FLOATING_POINT | RTEMS_LOCAL

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "commonheader.h"

//Execution flow begins here. This is referred to as the application
int main(void)
{
	rtems_status_code status[2];
	rtems_id ids[2], myid;
	rtems_task_priority priorities[2] = { 2, 3 };//VS { 5, 6 };
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

	//Obtaining current mode
	temp = rtems_task_mode(0, RTEMS_CURRENT_MODE, &oldmode);
	assert(temp == RTEMS_SUCCESSFUL);
	printf("Father task is preemptible: %d\n", oldmode & RTEMS_PREEMPT_MASK);

	//Changing current mode


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
	printf("qui1\n");
	status[1] = rtems_task_start(ids[1], task2_entrypoint, priorities[1]);
	printf("qui2\n");
	assert(status[0] == RTEMS_SUCCESSFUL);
	assert(status[1] == RTEMS_SUCCESSFUL);
	printf("2 tasks started successfully and they are ready\n");



	/*// prototype: rtems_task_delete(id);
	status[0] = rtems_task_delete(ids[0]);
	status[1] = rtems_task_delete(ids[1]);
	assert(status[0] == RTEMS_SUCCESSFUL);
	assert(status[1] == RTEMS_SUCCESSFUL);
	printf("2 tasks deleted successfully\n");*/


//busy wait
	while(1) {}
}

/* end of file */
