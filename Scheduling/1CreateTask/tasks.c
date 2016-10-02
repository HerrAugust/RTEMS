#include "commonheader.h"
#include <stdio.h>

rtems_task task1_entrypoint(rtems_task_argument priority)
{
	printf("This is the task with id %ld and with priority %d \n", RTEMS_SELF, priority);
}

rtems_task task2_entrypoint(rtems_task_argument priority)
{
	printf("This is the task with id %ld and with priority %d \n", RTEMS_SELF, priority);
}