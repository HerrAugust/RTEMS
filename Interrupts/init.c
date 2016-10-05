/*
 * In this example I show what the goal of interrupts priority is.
 * The father task creates a child task. The child task sends an interrupt of priority 77 to the father task, 
 * which in turn accepts it and begins not to accept interrupts with priority [77-255]. Then the child task
 * sends for the second time the same interrupt with priority 77 and the father task does not accept it.
 */

#include <stdio.h>
#include <stdlib.h>
#include <rtems.h>
#include <assert.h>

rtems_task childtask_entrypoint(rtems_task_argument fatherid)
{
	rtems_id id = rtems_task_self();
	rtems_task_priority priority;
	rtems_task_set_priority(id, RTEMS_CURRENT_PRIORITY, &priority); //returns current task priority
	printf("\tThis is the task with id %ld and with priority %ld\n", id, priority);

//send interrupt 77 to father

//send interrupt 77 to father

	//Without deleting this task, it will never return to father
	if(rtems_task_delete(id) != RTEMS_SUCCESSFUL) {
		printf("\tCannot delete task %ld", id);
	}
}


rtems_isr father_handler(rtems_vector_number vector)
{

}

rtems_task Init(rtems_task_argument ignored)
{
	rtems_status_code status, temp;
	rtems_id ids, myid;
	rtems_task_priority priorities = 2;
	rtems_name names = rtems_build_name('C', 'T', '1', ' ');
	rtems_task_priority p;

//Getting father's properties (because of a macro config. in commonheader.h, this is the first task executed and it is not pre-emptible and with priority 1)
	//Getting father's ID
	myid = rtems_task_self();
	printf("Father task has ID: %ld\n", myid);

	//Getting father's priority	(it is 4 because of CONFIGURE_INIT_TASK_PRIORITY)
	temp = rtems_task_set_priority(RTEMS_SELF, RTEMS_CURRENT_PRIORITY, &p); //returns current task priority
	assert(temp == RTEMS_SUCCESSFUL);
	printf("Father task has priority: %ld\n", p);

	//



//creating 1 child
	// prototype: rtems_task_create( name, initial_priority, stack_size, initial_modes, attribute_set, *id );
	status = rtems_task_create(names, priorities, RTEMS_MINIMUM_STACK_SIZE * 2,
		RTEMS_DEFAULT_MODES, RTEMS_DEFAULT_ATTRIBUTES, &ids);
	assert(status == RTEMS_SUCCESSFUL);
	printf("1 task created successfully (id: %ld), but it is dormant\n", ids);


	// prototype: rtems_task_start(id, entry_point, argument)
	status = rtems_task_start(ids, childtask_entrypoint, myid); //Notice: at this point, this task is pre-empted (because of the configuration below) and control goes to function task1_entrypoint()
	printf("here 1\n");
	assert(status == RTEMS_SUCCESSFUL);
	printf("1 task started successfully and is now ready\n");


//scheduling those two tasks
	rtems_task_wake_after(RTEMS_YIELD_PROCESSOR);
}

/* configuration information */

#include <bsp.h> /* for device driver prototypes */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MICROSECONDS_PER_TICK     1000 /* Each 1 millisecond a tick */
#define CONFIGURE_TICKS_PER_TIMESLICE		50   /* Each 50 milliseconds a context switch */

#define CONFIGURE_MAXIMUM_TASKS             2

#define CONFIGURE_EXTRA_TASK_STACKS         (6 * RTEMS_MINIMUM_STACK_SIZE)

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE //So default entry point is Init
#define CONFIGURE_INIT_TASK_PRIORITY 4
/*
#define CONFIGURE_INIT_TASK_INITIAL_MODES RTEMS_PREEMPT
*/

#define CONFIGURE_INIT
#include <rtems/confdefs.h>

/* end of file */
