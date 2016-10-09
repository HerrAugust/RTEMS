/*
 *  Simple test program -- simplified version of sample test hello.
 */

#include <stdio.h>
#include <stdlib.h>
#include <rtems.h>
#include <assert.h>
#include <unistd.h>
#include <time.h>


void periodicTask(rtems_task_argument ignored)
{
	rtems_status_code temp;
	rtems_name periodname = rtems_build_name('P', 'E', 'E', 'X');
	rtems_id periodid;
	
	printf("Note: Ticks in a second: %lu\n",rtems_clock_get_ticks_per_second());

//Initializing RMM
	temp = rtems_rate_monotonic_create(periodname, &periodid);
	assert(temp == RTEMS_SUCCESSFUL);


//Assigning this task to RMM with period 4 sec and performing some periodic action
	while(1)
	{
		//Starting period of 4 seconds
		temp = rtems_rate_monotonic_period(periodid, 4);
		if( temp == RTEMS_TIMEOUT )
			break;
		assert(temp == RTEMS_SUCCESSFUL);

		//Performing some periodic action
		printf("Periodic action\n\n");
	}

//Deleting istance of RMM and closing
	printf("Could not meet period. Deleting child\n");
	temp = rtems_rate_monotonic_delete(periodid);
	assert(temp == RTEMS_SUCCESSFUL);

	rtems_task_delete(rtems_task_self());
	perror("Could not delete periodic task. Error\n");
	exit(1);
}

void *POSIX_Init(void *argument)
{
	rtems_name cname = rtems_build_name('C', ' ', ' ', ' ');
	rtems_status_code temp;
	rtems_task_priority cpriority = 3;
	rtems_id childid;

	temp = rtems_task_create(cname, cpriority, RTEMS_MINIMUM_STACK_SIZE * 2,
		RTEMS_DEFAULT_MODES, RTEMS_DEFAULT_ATTRIBUTES, &childid);
	assert(temp == RTEMS_SUCCESSFUL);
	printf("Start child with priority %lu and id %lu\n", cpriority, childid);

	temp = rtems_task_start(childid, periodicTask, -1); //father preempted here
	if(temp != RTEMS_SUCCESSFUL)
	{
		perror("Could not start child\n");
		exit(2);
	}

	while(1) {}

	printf("Finishing father\n");
	exit(0);
}

/* configuration information */
#include <bsp.h>

/* NOTICE: the clock driver is explicitly disabled */
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER

#define CONFIGURE_POSIX_INIT_THREAD_TABLE
#define CONFIGURE_INIT_TASK_PRIORITY 4
#define CONFIGURE_INIT_TASK_INITIAL_MODES RTEMS_PREEMPT

#define CONFIGURE_MAXIMUM_POSIX_THREADS 1
#define CONFIGURE_MAXIMUM_TASKS             2

// Needed for RM Mangager
#define CONFIGURE_MAXIMUM_PERIODS           1
#define CONFIGURE_MICROSECONDS_PER_TICK     1000000 /* every 1 sec a tick */

#define CONFIGURE_INIT

#include <rtems/confdefs.h>

/* end of file */
