/*
 * In this example I show Priority Inheritance in RTEMS. In particular, it is centred on the evolution of priorities.
 * The father task creates two children. The second one blocks two shared variables.
 * The father is scheduled and tries to obtain resource SV1 but it may not, so it gets suspended.
 * The first first has the lowest priority and so gets scheduled. It tries to access SV0 but it may not, so
 * 		it gets suspended.
 * The second child is the only ready one and so it is scheduled. It has now priority 4 (4 = max(4,5,6) because of
 * priority inheritance). It releases both shared resources and terminates. 
 * The father task is scheduled because of its priority and it terminates.
 */

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <rtems.h>

rtems_id fatherid, childid[2];
rtems_id semid[2];
int shared_var, shared_var2; //say, bank account balances

rtems_task_priority changePriority(rtems_id taskid, rtems_task_priority newpriority)
{
	rtems_task_priority p;

	rtems_status_code temp = rtems_task_set_priority(taskid, newpriority, &p);
	assert(temp == RTEMS_SUCCESSFUL);

	return p;
}

rtems_task_priority getPriority(rtems_id taskid)
{
	rtems_task_priority p;

	rtems_status_code temp = rtems_task_set_priority(taskid, RTEMS_CURRENT_PRIORITY, &p);
	assert(temp == RTEMS_SUCCESSFUL);

	return p;
}

rtems_task task1_entrypoint(rtems_task_argument ignored)
{
	//coming from GOTO 3
	rtems_status_code temp;

	printf("\tChild 1 is going to acquire SV1. May not.\n");
	printf("\tChild 2 priority: %lu (exp. 4)\n", getPriority(childid[1]));

//Accessing resource SV1
	temp = rtems_semaphore_obtain(semid[0], RTEMS_WAIT, RTEMS_NO_TIMEOUT); //get suspended,so GOTO 4
	assert(temp == RTEMS_SUCCESSFUL);


	//Releasing SV1
	temp = rtems_semaphore_release(semid[0]);
   	assert(temp == RTEMS_SUCCESSFUL);

   	printf("\tChild 1 just released SV1. Child 2 priority: %lu (exp. 1)\n", getPriority(childid[1]));

   	printf("\tFinishing child 1\n");
   	//Without deleting this task, it will never return to father
	if(rtems_task_delete(childid[0]) != RTEMS_SUCCESSFUL) {
		printf("\tCannot delete task %ld\n", childid[0]);
	}
	//GOTO 4
}

rtems_task task2_entrypoint(rtems_task_argument ignored)
{
	//coming from GOTO 1
	rtems_status_code temp;
	//Restoring initial priority: 6
	changePriority(childid[1], 6);
	printf("\tChild 2: initial priority: %lu (exp. 6)\n", getPriority(childid[1]));

//Accessing resource SV2 and SV1
	temp = rtems_semaphore_obtain(semid[0], RTEMS_WAIT, RTEMS_NO_TIMEOUT);
	assert(temp == RTEMS_SUCCESSFUL);

	temp = rtems_semaphore_obtain(semid[1], RTEMS_WAIT, RTEMS_NO_TIMEOUT);
	assert(temp == RTEMS_SUCCESSFUL);

	printf("\tChild 2: acquired both shared resources. Priority: %lu (exp. 6)\n", getPriority(childid[1]));
	shared_var++;

//Scheduling Father
	rtems_task_wake_after(RTEMS_YIELD_PROCESSOR); //GOTO 2

//coming from GOTO 4
	printf("\tChild 2: priority: %lu (exp. 4)\n", getPriority(childid[1]));

   	temp = rtems_semaphore_release(semid[1]);
   	assert(temp == RTEMS_SUCCESSFUL);
   	printf("\tChild 2 just released SV2. Priority: %lu (exp. 5)\n", getPriority(childid[1]));

	temp = rtems_semaphore_release(semid[0]);
   	assert(temp == RTEMS_SUCCESSFUL);
   	printf("\tChild 2 just released also SV1. No semaphores active for Child 2.\n");
   	printf("\tChild 2: priority: %lu (exp. 6)\n", getPriority(childid[1]));


   	printf("\tFinishing child 2\n");
   	//Without deleting this task, it will never return to father
	if(rtems_task_delete(childid[1]) != RTEMS_SUCCESSFUL) {
		printf("\tCannot delete task %ld\n", childid[1]);
	} //GOTO 5
}

//Execution flow begins here. This is referred to as the application
rtems_task Init(rtems_task_argument argument)
{
	rtems_status_code status[2], temp;
	rtems_task_priority priorityc[2] = { 5, 6 };
	rtems_name childname[2] = { rtems_build_name('C', 'T', '1', ' '), rtems_build_name('C', 'T', '2', ' ') };
	rtems_name semname[2] = { rtems_build_name('S', '1', ' ', ' '), rtems_build_name('S', '2', ' ', ' ') };
	rtems_attribute mymode;

//Getting father's ID
	fatherid = rtems_task_self();
	printf("Father task has ID: %ld\n", fatherid);


//Creating mutex
	mymode = RTEMS_BINARY_SEMAPHORE | RTEMS_PRIORITY | RTEMS_INHERIT_PRIORITY | RTEMS_NO_PRIORITY_CEILING | RTEMS_NO_MULTIPROCESSOR_RESOURCE_SHARING | RTEMS_LOCAL;
	temp = rtems_semaphore_create(semname[0], 1, mymode , 2, &semid[0]); //priority_ceiling will be used to avoid to change father's priority 2 times
	assert(temp == RTEMS_SUCCESSFUL);
	temp = rtems_semaphore_create(semname[1], 1, mymode, 2, &semid[1]); //priority_ceiling will be used to avoid to change father's priority 2 times
	assert(temp == RTEMS_SUCCESSFUL);


//Creating 2 child
	// prototype: rtems_task_create( name, initial_priority, stack_size, initial_modes, attribute_set, *id );
	mymode = RTEMS_NO_PREEMPT | RTEMS_NO_TIMESLICE | RTEMS_ASR | RTEMS_INTERRUPT_LEVEL(0); //Note: must force RTEMS_NO_PREEMPT to succeed with this experiment
	status[0] = rtems_task_create(childname[0], priorityc[0], RTEMS_MINIMUM_STACK_SIZE * 2,
		mymode, RTEMS_DEFAULT_ATTRIBUTES, &childid[0]);
	status[1] = rtems_task_create(childname[1], priorityc[1], RTEMS_MINIMUM_STACK_SIZE * 2,
		mymode, RTEMS_DEFAULT_ATTRIBUTES, &childid[1]);
	assert(status[0] == RTEMS_SUCCESSFUL);
	assert(status[1] == RTEMS_SUCCESSFUL);
	printf("2 tasks created successfully (id: %ld and %ld), but they are dormant\n", childid[0], childid[1]);


	// prototype: rtems_task_start(id, entry_point, argument)
	status[0] = rtems_task_start(childid[0], task1_entrypoint, 0); //Notice: at this point, this task is pre-empted because of the configuration
	status[1] = rtems_task_start(childid[1], task2_entrypoint, 0); //Notice: at this point, this task is pre-empted because of the configuration
	assert(status[0] == RTEMS_SUCCESSFUL);
	assert(status[1] == RTEMS_SUCCESSFUL);

//Scheduling C2
	changePriority(childid[1], 3);
	rtems_task_wake_after(RTEMS_YIELD_PROCESSOR); //GOTO 1
	printf("Father task scheduled\n");

//coming from GOTO 2
//Trying to access SV2 with a mutex
	printf("Father task is going to access SV0. May not\n");
	temp = rtems_semaphore_obtain(semid[1], RTEMS_WAIT, RTEMS_NO_TIMEOUT); //father waits, so GOTO 3
	//coming from GOTO 5
	assert(temp == RTEMS_SUCCESSFUL);

//Releasing resource SV2
	temp = rtems_semaphore_release(semid[1]);
   	assert(temp == RTEMS_SUCCESSFUL);

	printf("Finishing father...\n");
}

#include <bsp.h> /* for device driver prototypes */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS             4

#define CONFIGURE_EXTRA_TASK_STACKS         (6 * RTEMS_MINIMUM_STACK_SIZE)

#define CONFIGURE_MICROSECONDS_PER_TICK     1000 /* Each 1 millisecond a tick */

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE //So default entry point is Init
#define CONFIGURE_INIT_TASK_PRIORITY 4

#define CONFIGURE_INIT
#include <rtems/confdefs.h>

/* end of file */
