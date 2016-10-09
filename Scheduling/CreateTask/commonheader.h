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



/* configuration information */

#include <bsp.h> /* for device driver prototypes */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MICROSECONDS_PER_TICK     500   // NB: 10 and lower gives system failure for erc32 simulator

#define CONFIGURE_MAXIMUM_TASKS             4

#define CONFIGURE_EXTRA_TASK_STACKS         (6 * RTEMS_MINIMUM_STACK_SIZE)

// Needed for RM Mangager
#define CONFIGURE_MAXIMUM_PERIODS           1

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE /* This results in a configuration of an application which will begin
											execution of a single initialization task named Init which is
											non-preemptible and at priority one (1).
										*/

#define CONFIGURE_INIT
#include <rtems/confdefs.h>
