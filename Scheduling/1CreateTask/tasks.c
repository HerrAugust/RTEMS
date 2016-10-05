#include "commonheader.h"
#include <stdio.h>

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