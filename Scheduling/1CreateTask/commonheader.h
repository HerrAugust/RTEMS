/*
 *
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */
/*  updated for triple test, 20003/11/06, Erik Adli */


#include <inttypes.h>
#include <rtems.h>

/* functions */

rtems_task Init(
  rtems_task_argument argument
);

rtems_task task1_entrypoint(
  rtems_task_argument argument
);

rtems_task task2_entrypoint(
  rtems_task_argument argument
);

/* global variables */

/*
 *  Keep the names and IDs in global variables so another task can use them.
 */ 
extern rtems_id   Task_id[ 4 ];         /* array of task ids */
extern rtems_name Task_name[ 4 ];       /* array of task names */



/* configuration information */

#include <bsp.h> /* for device driver prototypes */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MICROSECONDS_PER_TICK     500   // NB: 10 and lower gives system failure for erc32 simulator

#define CONFIGURE_MAXIMUM_TASKS             4

#define CONFIGURE_EXTRA_TASK_STACKS         (6 * RTEMS_MINIMUM_STACK_SIZE)

// Needed for RM Mangager
#define CONFIGURE_MAXIMUM_PERIODS           1

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE


/* Needed for erc32 simulator.. */
/* ..for using "CPU_usage_Dump", since it uses printf("%f") if your processor has floating points) */
/* If you want to take away FP support (to avoid heavy context switch), you must rewrite CPU_usage_Dump instead */
#define CONFIGURE_INIT_TASK_ATTRIBUTES RTEMS_FLOATING_POINT


#include <rtems/confdefs.h>
