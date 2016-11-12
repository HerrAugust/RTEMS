/*
 *  Simple test program -- simplified version of sample test hello.
 */

#include <stdio.h>
#include <stdlib.h>
#include <rtems.h>
#include <assert.h>
#include <unistd.h>
#include <time.h>
/* CPU usage and Rate monotonic manger statistics */
//#include "rtems/cpuuse.h"

double total_utilization;

void periodicTask1(rtems_task_argument ignored)
{
	rtems_status_code temp;
	rtems_name periodname = rtems_build_name('P', 'E', 'E', '1');
	rtems_id periodid;
	rtems_interval start;

	float computation_time = 0.0;
	int done = 0;

	printf("\tChild 1: 	period %d ms and computation time %d ms\n", 10, 6);

//Initializing RMM
	temp = rtems_rate_monotonic_create(periodname, &periodid);
	assert(temp == RTEMS_SUCCESSFUL);


//Assigning this task to RMM with period 10 msec and performing busy wait to simulate working time
	while(1)
	{
		//Starting period of 10 ms
		temp = rtems_rate_monotonic_period(periodid, 10);
		start = rtems_clock_get_ticks_since_boot();
		if( temp == RTEMS_TIMEOUT ) //if the body of this loop take more than 10 ms (i.e., periodic deadline missed), rtems_rate_monotonic_period returns RTEMS_TIMEOUT)
			break;
		assert(temp == RTEMS_SUCCESSFUL);

		//Simulating computation time (= 6 msec)
		while( (computation_time = rtems_clock_get_ticks_since_boot() - start) < 6) // 1 tick every ms => #tick/(#tick/ms) = #tick
			;

		if(!done) {
			printf("\tChild 1: comp_time = %f\n", computation_time);
			total_utilization += computation_time / 10.0; //10 is the period of this child
			done = 1;
		}
	}

//Deleting istance of RMM and closing
	printf("\tChild 1: Could not meet period. Deleting child\n");
	temp = rtems_rate_monotonic_delete(periodid);
	assert(temp == RTEMS_SUCCESSFUL);

	rtems_task_delete(rtems_task_self());
	perror("\tChild 1: Could not delete periodic task. Error\n");
	exit(1);
}

void periodicTask2(rtems_task_argument ignored)
{
	rtems_status_code temp;
	rtems_name periodname = rtems_build_name('P', 'E', 'E', '2');
	rtems_id periodid;
 	rtems_interval start;

	float computation_time = 0.0;
	int done = 0;

	printf("\t\tChild 2: period %d ms and computation time %d ms\n", 30, 9);
	
//Initializing RMM
	temp = rtems_rate_monotonic_create(periodname, &periodid);
	assert(temp == RTEMS_SUCCESSFUL);


//Assigning this task to RMM with period 30 ms and performing busy wait to simulate working time
	while(1)
	{
		//Starting period of 30 ms
		temp = rtems_rate_monotonic_period(periodid, 30);
		start = rtems_clock_get_ticks_since_boot();
		if( temp == RTEMS_TIMEOUT )  //if the body of this loop take more than 30 ms (i.e., periodic deadline missed), rtems_rate_monotonic_period returns RTEMS_TIMEOUT)
			break;
		assert(temp == RTEMS_SUCCESSFUL);

		//Simulating computation time (= 9 msec)
		while( (computation_time = rtems_clock_get_ticks_since_boot() - start) < 9)
			;

		if(!done) {
			printf("\t\tChild 2: comp_time = %f\n", computation_time);
			total_utilization += computation_time / 30.0; //30 is the period of this child
			printf("Total utilization: %f\n", total_utilization);
			done = 1;
		}
	}

//Deleting istance of RMM and closing
	printf("\t\tChild 2: Could not meet period. Deleting child\n");
	temp = rtems_rate_monotonic_delete(periodid);
	assert(temp == RTEMS_SUCCESSFUL);

	rtems_task_delete(rtems_task_self());
	perror("\t\tChild 2: Could not delete periodic task. Error\n");
	exit(1);
}

void *POSIX_Init(void *argument)
{
	rtems_name cname[2] = { rtems_build_name('C', '1', ' ', ' '), rtems_build_name('C', '2', ' ', ' ') };
	rtems_status_code temp;
	rtems_task_priority cpriority[2] = { 2, 2 };
	rtems_id childid[2];

	temp = rtems_task_create(cname[0], cpriority[0], RTEMS_MINIMUM_STACK_SIZE * 2,
		RTEMS_DEFAULT_MODES, RTEMS_DEFAULT_ATTRIBUTES & RTEMS_NO_FLOATING_POINT | RTEMS_FLOATING_POINT, &childid[0]);
	assert(temp == RTEMS_SUCCESSFUL);
	temp = rtems_task_create(cname[1], cpriority[1], RTEMS_MINIMUM_STACK_SIZE * 2,
		RTEMS_DEFAULT_MODES, RTEMS_DEFAULT_ATTRIBUTES & RTEMS_NO_FLOATING_POINT | RTEMS_FLOATING_POINT, &childid[1]);
	assert(temp == RTEMS_SUCCESSFUL);
	printf("Start child with id %lu\n", childid[0]);
	printf("Start child with id %lu\n", childid[1]);

	temp = rtems_task_start(childid[0], periodicTask1, -1); //father preempted here
	assert(temp == RTEMS_SUCCESSFUL);
	temp = rtems_task_start(childid[1], periodicTask2, -1); //father preempted here
	assert(temp == RTEMS_SUCCESSFUL);

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
#define CONFIGURE_INIT_TASK_PRIORITY 		6
#define CONFIGURE_INIT_TASK_INITIAL_MODES RTEMS_PREEMPT

#define CONFIGURE_MAXIMUM_POSIX_THREADS 	2
#define CONFIGURE_MAXIMUM_TASKS             3

// Needed for RM Mangager
#define CONFIGURE_MAXIMUM_PERIODS           2
#define CONFIGURE_MICROSECONDS_PER_TICK     1000 /* every millisec a tick */

#define CONFIGURE_TICKS_PER_TIMESLICE 		2 /* every 2 ticks a timeslice */

#define CONFIGURE_INIT

#include <rtems/confdefs.h>

/* end of file */
