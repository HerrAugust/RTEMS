#include "commonheader.h"
#include <stdio.h>

rtems_id id;

rtems_task task1_entrypoint(rtems_task_argument priority)
{
	id = rtems_task_self();
	printf("This is the task with id %ld and with priority %d\n", id, priority);
}

rtems_task task2_entrypoint(rtems_task_argument priority)
{
	id = rtems_task_self();
	printf("This is the task with id %ld and with priority %d\n", id, priority);
}