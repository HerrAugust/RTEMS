/*
 * In this example I show communication via semaphores.
 * Simply, the father task creates a child, which in turn accesses a shared variable with a mutex (i.e., binary semaphore).
 * Then, the father task tries to access the same variable. At the end, the variable will have expected
 * values.
 */

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <rtems.h>

rtems_id fatherid, childid[2];
rtems_id semid;
int shared_var; //say, bank account balance

rtems_task_priority changePriority(rtems_id taskid, rtems_task_priority newpriority)
{
	rtems_task_priority p;

	rtems_status_code temp = rtems_task_set_priority(taskid, 4, &p); //otherwise children could not be scheduled
	assert(temp == RTEMS_SUCCESSFUL);

	return p;
}

rtems_task task1_entrypoint(rtems_task_argument ignored)
{
	rtems_status_code temp;
	

   	printf("\tFinishing child 1\n");
   	//Without deleting this task, it will never return to father
	if(rtems_task_delete(childid[0]) != RTEMS_SUCCESSFUL) {
		printf("\tCannot delete task %ld", childid[0]);
	}
}

rtems_task task2_entrypoint(rtems_task_argument ignored)
{
	rtems_status_code temp;
	

   	printf("\tFinishing child 2\n");
   	//Without deleting this task, it will never return to father
	if(rtems_task_delete(childid[1]) != RTEMS_SUCCESSFUL) {
		printf("\tCannot delete task %ld", childid[1]);
	}
}

//Execution flow begins here. This is referred to as the application
rtems_task Init(rtems_task_argument argument)
{
	rtems_status_code status[2], temp;
	rtems_task_priority priorityc[2] = { 3, 2 };
	rtems_name childname[2] = { rtems_build_name('C', 'T', '1', ' '), rtems_build_name('C', 'T', '2', ' ') };
	rtems_name semname = rtems_build_name('S', '1', ' ', ' ');

//Getting father's ID
	fatherid = rtems_task_self();
	printf("Father task has ID: %ld\n", fatherid);


//Creating mutex
	temp = rtems_semaphore_create(semname, 1, RTEMS_DEFAULT_ATTRIBUTES, 2, &semid); //priority_ceiling will be used to avoid to change father's priority 2 times
	assert(temp == RTEMS_SUCCESSFUL);


//Creating 2 child
	// prototype: rtems_task_create( name, initial_priority, stack_size, initial_modes, attribute_set, *id );
	status[0] = rtems_task_create(childname[0], priorityc[0], RTEMS_MINIMUM_STACK_SIZE * 2,
		RTEMS_DEFAULT_MODES, RTEMS_DEFAULT_ATTRIBUTES, &childid[0]);
	status[1] = rtems_task_create(childname[1], priorityc[1], RTEMS_MINIMUM_STACK_SIZE * 2,
		RTEMS_DEFAULT_MODES, RTEMS_DEFAULT_ATTRIBUTES, &childid[1]);
	assert(status[0] == RTEMS_SUCCESSFUL);
	assert(status[1] == RTEMS_SUCCESSFUL);
	printf("2 tasks created successfully (id: %ld and %ld), but they are dormant\n", childid[0], childid[1]);


	// prototype: rtems_task_start(id, entry_point, argument)
	status[0] = rtems_task_start(childid[0], task1_entrypoint, 0); //Notice: at this point, this task is pre-empted because of the configuration
	status[1] = rtems_task_start(childid[1], task2_entrypoint, 0); //Notice: at this point, this task is pre-empted because of the configuration
	assert(status[0] == RTEMS_SUCCESSFUL);
	assert(status[1] == RTEMS_SUCCESSFUL);


//Accessing file with a mutex
	temp = rtems_semaphore_obtain(semid, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
	assert(temp == RTEMS_SUCCESSFUL);

//Performing atomic operation
	shared_var = shared_var + 100;


//Releasing mutex
   	temp = rtems_semaphore_release(semid);
   	assert(temp == RTEMS_SUCCESSFUL);
   	printf("Father wrote\n");

	printf("Finishing father...");
	printf("Value of shared variable = %d (exp. 1000-500+100 = 600)\n", shared_var);
}

#include <bsp.h> /* for device driver prototypes */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS             4

#define CONFIGURE_EXTRA_TASK_STACKS         (6 * RTEMS_MINIMUM_STACK_SIZE)

#define CONFIGURE_MICROSECONDS_PER_TICK     1000 /* Each 1 millisecond a tick */

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE //So default entry point is Init
#define CONFIGURE_INIT_TASK_PRIORITY 4
#define CONFIGURE_INIT_TASK_INITIAL_MODES RTEMS_PREEMPT

#define CONFIGURE_INIT
#include <rtems/confdefs.h>

/* end of file */
