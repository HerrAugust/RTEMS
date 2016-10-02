/*
 * The application starts a task.
 * The task must print "I am a task and I am running"
 */

//#include <bsp.h>

#include <stdio.h>
#include <stdlib.h>



void *POSIX_Init( //Execution flow begins here. This is referred to as the application
  void *argument
)
{
  printf( "\n\n*** Creating a task ***\n" );
  void rtems_initialize_executive(void);



  exit( 0 );
}

/* configuration information */
#include <bsp.h>

/* NOTICE: the clock driver is explicitly disabled */
#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER

#define CONFIGURE_POSIX_INIT_THREAD_TABLE

#define CONFIGURE_MAXIMUM_POSIX_THREADS 1

#define CONFIGURE_INIT

#include <rtems/confdefs.h>

/* end of file */
