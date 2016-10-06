/*
 * In this example I show message passing (IPC).
 * The father task creates two childrem and sends to them two different messages.
 * Children reply and father receives their replies.
 */

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <rtems.h>
#include <string.h>

rtems_id queueid;

rtems_task task1_entrypoint(rtems_task_argument ignored)
{
	rtems_status_code temp;
	char msg[40];
	size_t size;
	char reply[] = "Child 1: hi father!";

	temp = rtems_message_queue_receive(queueid, &msg, &size, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
	assert(temp == RTEMS_SUCCESSFUL);
	printf("\tChild 1 received something: %s\n", msg);

	temp = rtems_message_queue_send(queueid, reply, strlen(reply));
	assert(temp == RTEMS_SUCCESSFUL);

	printf("\tChild 1 sent the reply\n");
	assert(RTEMS_SUCCESSFUL == rtems_task_delete(rtems_task_self()));
}

rtems_task task2_entrypoint(rtems_task_argument ignored)
{
	rtems_status_code temp;
	char msg[40];
	size_t size;
	char reply[] = "Child 2: hi father!";

	temp = rtems_message_queue_receive(queueid, msg, &size, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
	assert(temp == RTEMS_SUCCESSFUL);
	printf("\tChild 2 received something: %s\n", msg);

	temp = rtems_message_queue_send(queueid, reply, strlen(reply));
	assert(temp == RTEMS_SUCCESSFUL);

	printf("\tChild 2 sent the reply\n");
	assert(RTEMS_SUCCESSFUL == rtems_task_delete(rtems_task_self()));
}

//Execution flow begins here. This is referred to as the application
rtems_task Init(rtems_task_argument argument)
{
	rtems_status_code status[2], temp;
	rtems_id ids[2], myid;
	rtems_task_priority priorities[2] = { 2, 3 };
	rtems_name names[2];
	rtems_task_priority p;

	names[0] = rtems_build_name('C', 'T', '1', ' ');
	names[1] = rtems_build_name('C', 'T', '2', ' ');

//Getting father's properties
	//Getting father's ID
	myid = rtems_task_self();
	printf("Father task has ID: %ld\n", myid);

	//Changin father's priority	
	temp = rtems_task_set_priority(RTEMS_SELF, RTEMS_CURRENT_PRIORITY, &p); //returns current task priority
	assert(temp == RTEMS_SUCCESSFUL);
	printf("Father task has priority: %ld\n", p);


//creating 2 children
	// prototype: rtems_task_create( name, initial_priority, stack_size, initial_modes, attribute_set, *id );
	status[0] = rtems_task_create(names[0], priorities[0], RTEMS_MINIMUM_STACK_SIZE * 2,
		RTEMS_DEFAULT_MODES, RTEMS_DEFAULT_ATTRIBUTES, &ids[0]);
	status[1] = rtems_task_create(names[1], priorities[1], RTEMS_MINIMUM_STACK_SIZE * 2,
		RTEMS_DEFAULT_MODES, RTEMS_DEFAULT_ATTRIBUTES, &ids[1]);
	assert(status[0] == RTEMS_SUCCESSFUL);
	assert(status[1] == RTEMS_SUCCESSFUL);
	printf("2 tasks created successfully (id: %ld and %ld), but they are dormant\n", ids[0], ids[1]);


	// prototype: rtems_task_start(id, entry_point, argument)
	status[0] = rtems_task_start(ids[0], task1_entrypoint, priorities[0]); //Notice: at this point, this task is not pre-empted because of the default configuration
	status[1] = rtems_task_start(ids[1], task2_entrypoint, priorities[1]);
	assert(status[0] == RTEMS_SUCCESSFUL);
	assert(status[1] == RTEMS_SUCCESSFUL);
	printf("2 tasks started successfully and they are ready\n");

//Sending messages
	// rtems_status_code rtems_message_queue_create(name, count, max_message_size, attribute_set, *id);
	rtems_name queuename = rtems_build_name( 'Q', '1', ' ', ' ' );
	size_t size;
	char msgc1[] = "Hi, my son", msgc2[] = "Hi, my daughter";
	char replyc1[40], replyc2[40];

	temp = rtems_message_queue_create(queuename, 10, 128, RTEMS_DEFAULT_ATTRIBUTES, &queueid);
	assert(temp == RTEMS_SUCCESSFUL);

	// rtems_message_queue_send(id, *buffer, size);
	temp = rtems_message_queue_send(queueid, msgc1, strlen(msgc1));
	assert(temp == RTEMS_SUCCESSFUL);
	temp = rtems_message_queue_send(queueid, msgc2, strlen(msgc2));
	assert(temp == RTEMS_SUCCESSFUL);
	printf("Father sent two messages. Now it waits for replies.\n");

	//without this operation, father would read his own messages
	rtems_task_wake_after(RTEMS_YIELD_PROCESSOR);

	temp = rtems_message_queue_receive(queueid, replyc1, &size, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
	assert(temp == RTEMS_SUCCESSFUL);
	printf("Father received message: %s\n", replyc1);
	temp = rtems_message_queue_receive(queueid, replyc2, &size, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
	assert(temp == RTEMS_SUCCESSFUL);
	printf("Father received message: %s\n", replyc2);

	temp = rtems_message_queue_flush(queueid, NULL);
	assert(temp == RTEMS_INVALID_ADDRESS);

	printf("Finishing\n");
}

#include <bsp.h> /* for device driver prototypes */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS             4

#define CONFIGURE_EXTRA_TASK_STACKS         (6 * RTEMS_MINIMUM_STACK_SIZE)

#define CONFIGURE_MICROSECONDS_PER_TICK     1000 /* Each 1 millisecond a tick */

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE //So default entry point is Init
#define CONFIGURE_INIT_TASK_PRIORITY 4
//#define CONFIGURE_INIT_TASK_INITIAL_MODES RTEMS_PREEMPT

#define CONFIGURE_MAXIMUM_MESSAGE_QUEUES 1
#define CONFIGURE_MESSAGE_BUFFER_MEMORY \
CONFIGURE_MESSAGE_BUFFERS_FOR_QUEUE(12, sizeof(uint32_t))

#define CONFIGURE_INIT
#include <rtems/confdefs.h>

/* end of file */
