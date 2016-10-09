/*
 *  Simple test program -- simplified version of sample test hello.
 */

#include <stdio.h>
#include <stdlib.h>
#include <rtems.h>
#include <assert.h>
#include <unistd.h>


void periodicTask(rtems_task_argument ignored)
{
	

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
printf("PROCESSORI: %lu\n", rtems_get_processor_count());
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

	printf("Finishing father\n");
	exit(0);
}

/* configuration information */
#include <bsp.h>

/* NOTICE: the clock driver is explicitly disabled */
#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER

#define CONFIGURE_POSIX_INIT_THREAD_TABLE
#define CONFIGURE_INIT_TASK_PRIORITY 4
#define CONFIGURE_INIT_TASK_INITIAL_MODES RTEMS_PREEMPT

#define CONFIGURE_MAXIMUM_POSIX_THREADS 1
#define CONFIGURE_MAXIMUM_TASKS             4

#define CONFIGURE_MICROSECONDS_PER_TICK     500

#define CONFIGURE_SMP_APPLICATION
#define CONFIGURE_SMP_MAXIMUM_PROCESSORS 4

#define CONFIGURE_INIT

#include <rtems/confdefs.h>

/* end of file */
