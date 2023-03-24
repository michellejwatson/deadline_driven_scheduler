/*
    FreeRTOS V9.0.0 - Copyright (C) 2016 Real Time Engineers Ltd.
    All rights reserved

    VISIT http://www.FreeRTOS.org TO ENSURE YOU ARE USING THE LATEST VERSION.

    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation >>>> AND MODIFIED BY <<<< the FreeRTOS exception.

    ***************************************************************************
    >>!   NOTE: The modification to the GPL is included to allow you to     !<<
    >>!   distribute a combined work that includes FreeRTOS without being   !<<
    >>!   obliged to provide the source code for proprietary components     !<<
    >>!   outside of the FreeRTOS kernel.                                   !<<
    ***************************************************************************

    FreeRTOS is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  Full license text is available on the following
    link: http://www.freertos.org/a00114.html

    ***************************************************************************
     *                                                                       *
     *    FreeRTOS provides completely free yet professionally developed,    *
     *    robust, strictly quality controlled, supported, and cross          *
     *    platform software that is more than just the market leader, it     *
     *    is the industry's de facto standard.                               *
     *                                                                       *
     *    Help yourself get started quickly while simultaneously helping     *
     *    to support the FreeRTOS project by purchasing a FreeRTOS           *
     *    tutorial book, reference manual, or both:                          *
     *    http://www.FreeRTOS.org/Documentation                              *
     *                                                                       *
    ***************************************************************************

    http://www.FreeRTOS.org/FAQHelp.html - Having a problem?  Start by reading
    the FAQ page "My application does not run, what could be wwrong?".  Have you
    defined configASSERT()?

    http://www.FreeRTOS.org/support - In return for receiving this top quality
    embedded software for free we request you assist our global community by
    participating in the support forum.

    http://www.FreeRTOS.org/training - Investing in training allows your team to
    be as productive as possible as early as possible.  Now you can receive
    FreeRTOS training directly from Richard Barry, CEO of Real Time Engineers
    Ltd, and the world's leading authority on the world's leading RTOS.

    http://www.FreeRTOS.org/plus - A selection of FreeRTOS ecosystem products,
    including FreeRTOS+Trace - an indispensable productivity tool, a DOS
    compatible FAT file system, and our tiny thread aware UDP/IP stack.

    http://www.FreeRTOS.org/labs - Where new FreeRTOS products go to incubate.
    Come and try FreeRTOS+TCP, our new open source TCP/IP stack for FreeRTOS.

    http://www.OpenRTOS.com - Real Time Engineers ltd. license FreeRTOS to High
    Integrity Systems ltd. to sell under the OpenRTOS brand.  Low cost OpenRTOS
    licenses offer ticketed support, indemnification and commercial middleware.

    http://www.SafeRTOS.com - High Integrity Systems also provide a safety
    engineered and independently SIL3 certified version for use in safety and
    mission critical applications that require provable dependability.

    1 tab == 4 spaces!
*/

/*
FreeRTOS is a market leading RTOS from Real Time Engineers Ltd. that supports
31 architectures and receives 77500 downloads a year. It is professionally
developed, strictly quality controlled, robust, supported, and free to use in
commercial products without any requirement to expose your proprietary source
code.

This simple FreeRTOS demo does not make use of any IO ports, so will execute on
any Cortex-M3 of Cortex-M4 hardware.  Look for TODO markers in the code for
locations that may require tailoring to, for example, include a manufacturer
specific header file.

This is a starter project, so only a subset of the RTOS features are
demonstrated.  Ample source comments are provided, along with web links to
relevant pages on the http://www.FreeRTOS.org site.

Here is a description of the project's functionality:

The main() Function:
main() creates the tasks and software timers described in this section, before
starting the scheduler.

The Queue Send Task:
The queue send task is implemented by the prvQueueSendTask() function.
The task uses the FreeRTOS vTaskDelayUntil() and xQueueSend() API functions to
periodically send the number 100 on a queue.  The period is set to 200ms.  See
the comments in the function for more details.
http://www.freertos.org/vtaskdelayuntil.html
http://www.freertos.org/a00117.html

The Queue Receive Task:
The queue receive task is implemented by the prvQueueReceiveTask() function.
The task uses the FreeRTOS xQueueReceive() API function to receive values from
a queue.  The values received are those sent by the queue send task.  The queue
receive task increments the ulCountOfItemsReceivedOnQueue variable each time it
receives the value 100.  Therefore, as values are sent to the queue every 200ms,
the value of ulCountOfItemsReceivedOnQueue will increase by 5 every second.
http://www.freertos.org/a00118.html

An example software timer:
A software timer is created with an auto reloading period of 1000ms.  The
timer's callback function increments the ulCountOfTimerCallbackExecutions
variable each time it is called.  Therefore the value of
ulCountOfTimerCallbackExecutions will count seconds.
http://www.freertos.org/RTOS-software-timer.html

The FreeRTOS RTOS tick hook (or callback) function:
The tick hook function executes in the context of the FreeRTOS tick interrupt.
The function 'gives' a semaphore every 500th time it executes.  The semaphore
is used to synchronise with the event semaphore task, which is described next.

The event semaphore task:
The event semaphore task uses the FreeRTOS xSemaphoreTake() API function to
wait for the semaphore that is given by the RTOS tick hook function.  The task
increments the ulCountOfReceivedSemaphores variable each time the semaphore is
received.  As the semaphore is given every 500ms (assuming a tick frequency of
1KHz), the value of ulCountOfReceivedSemaphores will increase by 2 each second.

The idle hook (or callback) function:
The idle hook function queries the amount of free FreeRTOS heap space available.
See vApplicationIdleHook().

The malloc failed and stack overflow hook (or callback) functions:
These two hook functions are provided as examples, but do not contain any
functionality.
*/

/* Standard includes. */
#include <stdint.h>
#include <stdio.h>
//#include <stdlib.h>
#include "stm32f4_discovery.h"

/* Kernel includes. */
#include "stm32f4xx.h"
#include "../FreeRTOS_Source/include/FreeRTOS.h"
#include "../FreeRTOS_Source/include/queue.h"
#include "../FreeRTOS_Source/include/semphr.h"
#include "../FreeRTOS_Source/include/task.h"
#include "../FreeRTOS_Source/include/timers.h"
/* I think this should switch it to use heap 4 memory */
/* ended up making heap1 use heap4 code */
//#include "../FreeRTOS_Source/portable/MemMang/heap_4.c"

/*-----------------------------------------------------------*/
//#define mainQUEUE_LENGTH 100
#define QUEUE_LENGTH 100

#define amber_led	LED3
#define green_led	LED4
#define blue_led	LED6

/* test bench 1 */
#define TASK_1_EXECUTION_TIME 95
#define TASK_1_PERIOD 500
#define TASK_2_EXECUTION_TIME 150
#define TASK_2_PERIOD 500
#define TASK_3_EXECUTION_TIME 250
#define TASK_3_PERIOD 750

/* test bench 2
#define TASK_1_EXECUTION_TIME 95
#define TASK_1_PERIOD 250
#define TASK_2_EXECUTION_TIME 150
#define TASK_1_PERIOD 500
#define TASK_3_EXECUTION_TIME 250
#define TASK_1_PERIOD 750 */

/* test bench 3
#define TASK_1_EXECUTION_TIME 100
#define TASK_1_PERIOD 500
#define TASK_2_EXECUTION_TIME 200
#define TASK_1_PERIOD 500
#define TASK_3_EXECUTION_TIME 200
#define TASK_1_PERIOD 500 */

/* Initializer functions for hardware */
static void prvSetupHardware( void );

/* data types */
enum task_type {PERIODIC,APERIODIC};

struct dd_task {
	TaskHandle_t t_handle;
	enum task_type type;
	uint32_t task_id;
	uint32_t release_time;
	uint32_t absolute_deadline;
	uint32_t completion_time;
};

typedef struct dd_task_list {
	struct dd_task task;
	struct dd_task_list *next_task;
} dd_task_list;

enum message_type {create_dd, delete_dd, get_active_dd_list, get_completed_dd_list, get_overdue_dd_list};

typedef struct message {
	enum message_type type;
	struct dd_task *task_info;
	uint32_t task_id;
	struct dd_task_list *task_list;

} message;

/* helper functions */
void create_dd_task( TaskHandle_t t_handle, enum task_type type, uint32_t task_id, uint32_t absolute_deadline);
void delete_dd_task(uint32_t task_id);
struct dd_task_list * get_active_dd_task_list(void);
struct dd_task_list * get_complete_dd_task_list(void);
struct dd_task_list * get_overdue_dd_task_list(void);

/* create linked lists */
/*struct dd_task_list* active_list = NULL;
struct dd_task_list* complete_list = NULL;
struct dd_task_list* overdue_list = NULL;*/

/* linked list functions */
dd_task_list * add_task( struct dd_task_list * list_head, struct dd_task * new_dd_task );
dd_task_list * delete_task( struct dd_task_list * list_head, struct dd_task * done_dd_task );
int print_count_of_list ( struct dd_task_list* dd_task_list_head );
dd_task_list * order_tasks_deadline_first( dd_task_list* dd_task_list_head );
dd_task_list * assign_task_priorities ( dd_task_list* dd_task_list_head );
struct dd_task get_dd_task(struct dd_task_list * list_head, uint32_t task_id);

/* tasks */
static void DD_Scheduler_Task( void *pvParameters );
static void DD_Task_Generator_1( void *pvParameters );
static void DD_Task_Generator_2( void *pvParameters );
static void DD_Task_Generator_3( void *pvParameters );
static void User_Task_1( void *pvParameters );
static void User_Task_2( void *pvParameters );
static void User_Task_3( void *pvParameters );
static void Monitor_Task( void *pvParameters );

/* task handlers */
TaskHandle_t xGenerator1TaskHandle = NULL;
TaskHandle_t xGenerator2TaskHandle = NULL;
TaskHandle_t xGenerator3TaskHandle = NULL;

/* queues */
xQueueHandle xQueue_request_handle = 0;
xQueueHandle xQueue_response_handle = 0;

/* Initialize software timers */
TimerHandle_t xGenerator1Timer;
TimerHandle_t xGenerator2Timer;
TimerHandle_t xGenerator3Timer;

void vGenerator1CallbackFunction( TimerHandle_t xGenerator1Timer );
void vGenerator2CallbackFunction( TimerHandle_t xGenerator2Timer );
void vGenerator3CallbackFunction( TimerHandle_t xGenerator3Timer );

/*-----------------------------------------------------------*/

int main(void)
{
	/* Configure the system. */
	prvSetupHardware();

	STM_EVAL_LEDInit(amber_led); 
	STM_EVAL_LEDInit(green_led);
	STM_EVAL_LEDInit(blue_led);

	// create queue to send and receive potentiometer value
	xQueue_request_handle = xQueueCreate( QUEUE_LENGTH, sizeof(message *));
	xQueue_response_handle = xQueueCreate( QUEUE_LENGTH, sizeof(message *));

	// add potentiometer queue to registry for debugging
	vQueueAddToRegistry( xQueue_request_handle, "RequestQueue" );
	vQueueAddToRegistry( xQueue_response_handle, "ResponseQueue" );

	// need to decide priority and stack size
	xTaskCreate(DD_Scheduler_Task, "DD Scheduler", configMINIMAL_STACK_SIZE, NULL, 4, NULL);
	xTaskCreate(DD_Task_Generator_1, "DD Task Generator 1", configMINIMAL_STACK_SIZE, NULL, 3, xGenerator1TaskHandle);
	//xTaskCreate(DD_Task_Generator_2, "DD Task Generator 2", configMINIMAL_STACK_SIZE, NULL, 3, xGenerator2TaskHandle);
	//xTaskCreate(DD_Task_Generator_3, "DD Task Generator 3", configMINIMAL_STACK_SIZE, NULL, 3, xGenerator3TaskHandle);
	xTaskCreate(Monitor_Task, "Monitor", configMINIMAL_STACK_SIZE, NULL, 2, NULL);

	// create software timers for controlling traffic lights
	xGenerator1Timer = xTimerCreate("Generator 1", TASK_1_PERIOD / portTICK_PERIOD_MS, pdTRUE, (void *) 0, vGenerator1CallbackFunction);
	xGenerator2Timer = xTimerCreate("Generator 2", TASK_2_PERIOD  / portTICK_PERIOD_MS, pdTRUE, (void *) 0, vGenerator2CallbackFunction);
	xGenerator3Timer = xTimerCreate("Generator 3", TASK_3_PERIOD / portTICK_PERIOD_MS, pdTRUE, (void *) 0, vGenerator3CallbackFunction);

	/* Start the tasks and timer running. */
	vTaskStartScheduler();

	return 0;
}


/*------------Tasks-----------------------------------------------*/
static void DD_Scheduler_Task( void *pvParameters )
{
	// waits for scheduling task (usually should be suspended until scheduling task exists)

	// initialize task lists (only available to scheduler)
	struct dd_task_list* active_list = NULL;
	struct dd_task_list* complete_list = NULL;
	struct dd_task_list* overdue_list = NULL;

	while(1)
	{
		message* received_message;// = (message *)pvPortMalloc(sizeof(message *));
		//received_message->task_info = (struct dd_task *)pvPortMalloc(sizeof(struct dd_task *));
		//received_message->type = (enum message_type)pvPortMalloc(sizeof(enum message_type));
		//received_message->task_list = (struct dd_task_list *)pvPortMalloc(sizeof(struct dd_task_list *));
		// add check for overdue tasks (could use software timers instead, would be better to accomodate for aperiodic but kinda redundant for periodic)

		// adjusts user task priorities  (Set earliest deadline task priority to high and the rest to low so first completes that and then so on)
		if(xQueueReceive(xQueue_request_handle, &received_message, 500)){
			// cases based on what message type is received
			if (received_message->type == create_dd)
			{
				// assign release time to new task
				//struct dd_task *new_task = (struct dd_task *)pvPortMalloc(sizeof(struct dd_task *));
				//new_task = received_message->task_info;
				//new_task->release_time = xTaskGetTickCount();
				//new_task->completion_time = 0;
				 received_message->task_info->release_time = xTaskGetTickCount();
				 received_message->task_info->completion_time = 0;

				// add DD task to active task list
				active_list = add_task(active_list, received_message->task_info);

				// sort list by deadline
				active_list = order_tasks_deadline_first(active_list);

				// set priorities of user defined task according to sorted list
				// head (first of list) set to high priority, rest set to low priority
				active_list = assign_task_priorities(active_list);

				//vPortFree(received_message);

				// start task
				vTaskResume(received_message->task_info->t_handle);

				// MAYBE: start software timer, callback function will be to check if overdue time is passed, when callback is called: send new message type (overdue_dd_task)
			}
			else if (received_message->type == delete_dd)
			{
				// assign completion time to newly completed DD-task
				//struct dd_task done_task = *received_message->task_info;
				struct dd_task done_task = get_dd_task(active_list, received_message->task_id);

				done_task.completion_time = xTaskGetTickCount();

				// remove DD task from active task list
				active_list = delete_task(active_list, &done_task);

				// add DD task to complete task list
				complete_list = add_task(complete_list, &done_task);

				// re-sort active task list by deadline
				active_list = order_tasks_deadline_first(active_list);

				// set priorities of user defined tasks accordingly
				// head (first of list) set to high priority, rest set to low priority
				active_list = assign_task_priorities(active_list);
			}
			/* else if (message.message_type = overdue_dd_task)
			* {
			* 		//from call back function
			* 		// remove DD task from active task list
			* 		// add DD task to overdue task list
			* 		// re-sort overdue task list by deadline
			* 		// set priorities of user defined tasks accordingly
			* }
			*/
			else if (received_message->type == get_active_dd_list)
			{
				received_message->task_list = active_list;

				if (xQueueSend(xQueue_response_handle, &received_message, 500)){
					// sent message successfully
				}

				//vPortFree(received_message);
			}
			else if (received_message->type == get_completed_dd_list)
			{
				// i think this should be handled by message with delete_dd ?
				/*struct dd_task_list *currentList = active_list;
				while (currentList != NULL) {
					// if completion_time is 0, that means task hasn't been completed
					if( currentList->task.completion_time != 0) {
						complete_list = add_task(complete_list, &currentList->task);
						// delete the completed task from active task list
						active_list = delete_task(active_list, &currentList->task);
						currentList = currentList->next_task;
					}
				}*/

				received_message->task_list = complete_list;

				// get complete task list and send to a queue
				if (xQueueSend(xQueue_response_handle, &received_message, 500)){
					// sent message successfully
				}

				//vPortFree(received_message);
			}
			else if (received_message->type == get_overdue_dd_list)
			{
				/*struct dd_task_list *currentList = active_list;
				uint32_t timeRightNow = xTaskGetTickCount();

				while (currentList != NULL) {
					// if completion_time is 0, that means task hasn't been completed
					if( currentList->task.absolute_deadline <= timeRightNow) {
						overdue_list = add_task(overdue_list, &currentList->task);
						// delete overdue task from active task list
						active_list = delete_task(active_list, &currentList->task);
						currentList = currentList->next_task;
					}
				}*/

				received_message->task_list = overdue_list;

				// get complete task list and send to a queue
				if (xQueueSend(xQueue_response_handle, &received_message, 500)){
					// sent message successfully
				}

				//vPortFree(received_message);
			}
		}
	}
}

// task generator for DD_User_Task_1 (will have one for each DD task)
static void DD_Task_Generator_1( void *pvParameters )
{
	// normal state: suspended, resumed when software timer callback is triggered, timer period would be TASK_1_PERIOD
	// should reuse F-Task handles inside each DD task
	// periodically generate user tasks for DD_Scheduler
	TaskHandle_t task_1_handle;
	xTaskCreate( User_Task_1, "User Task 1", configMINIMAL_STACK_SIZE, NULL, 1, &task_1_handle); // start it at lowest priority
	vTaskSuspend(task_1_handle); // DD scheduler will start it when it should be run

	// prepares all information needed for creating specific instance of DD-tasks
	// calls create_dd_task
	// xTaskGetTickCount: The count of ticks since vTaskStartScheduler was called
	create_dd_task(task_1_handle, PERIODIC, 1,  xTaskGetTickCount() + TASK_1_EXECUTION_TIME / portTICK_PERIOD_MS);

	// needs to suspend itself (NULL means suspends itself)
	vTaskSuspend( NULL );
}

// task generator for DD_User_Task_1 (will have one for each DD task)
static void DD_Task_Generator_2( void *pvParameters )
{
	// normal state: suspended, resumed when software timer callback is triggered, timer period would be TASK_2_PERIOD
	// should reuse F-Task handles inside each DD task
	// periodically generate user tasks for DD_Scheduler
	TaskHandle_t task_2_handle;
	xTaskCreate( User_Task_2, "User Task 2", configMINIMAL_STACK_SIZE, NULL, 1, &task_2_handle); // start it at lowest priority
	vTaskSuspend(task_2_handle); // DD scheduler will start it when it should be run

	// prepares all information needed for creating specific instance of DD-tasks
	// calls create_dd_task
	// xTaskGetTickCount: The count of ticks since vTaskStartScheduler was called
	create_dd_task(task_2_handle, PERIODIC, 2,  xTaskGetTickCount() + TASK_2_EXECUTION_TIME / portTICK_PERIOD_MS);

	// needs to suspend itself (NULL means suspends itself)
	vTaskSuspend( NULL );
}

// task generator for DD_User_Task_1 (will have one for each DD task)
static void DD_Task_Generator_3( void *pvParameters )
{
	// normal state: suspended, resumed when software timer callback is triggered, timer period would be TASK_3_PERIOD
	// should reuse F-Task handles inside each DD task
	// periodically generate user tasks for DD_Scheduler
	TaskHandle_t task_3_handle;
	xTaskCreate( User_Task_3, "User Task 3", configMINIMAL_STACK_SIZE, NULL, 1, &task_3_handle); // start it at lowest priority
	vTaskSuspend(task_3_handle); // DD scheduler will start it when it should be run

	// prepares all information needed for creating specific instance of DD-tasks
	// calls create_dd_task
	// xTaskGetTickCount: The count of ticks since vTaskStartScheduler was called
	create_dd_task(task_3_handle, PERIODIC, 3,  xTaskGetTickCount() + TASK_3_EXECUTION_TIME / portTICK_PERIOD_MS);

	// needs to suspend itself (NULL means suspends itself)
	vTaskSuspend( NULL );
}

static void User_Task_1( void *pvParameters )
{
	TickType_t start_time = xTaskGetTickCount();
	TickType_t end_time = start_time + TASK_1_EXECUTION_TIME / portTICK_PERIOD_MS;

	// could turn on LED on discovery board to provide visual indication of what user-defined task is executing
	STM_EVAL_LEDOn(amber_led);

	// execute an empty loop for duration of execution time
	/*while ( end_time > xTaskGetTickCount())
	{
		// loop for TASK_1_EXECUTION_TIME
	}*/

	// turn off LED here
	STM_EVAL_LEDOff(amber_led);

	// execution time is done, delete task
	delete_dd_task(1);

	// ends its task
	vTaskDelete(NULL);
}

static void User_Task_2( void *pvParameters )
{
	TickType_t start_time = xTaskGetTickCount();
	TickType_t end_time = start_time + TASK_2_EXECUTION_TIME / portTICK_PERIOD_MS;

	// could turn on LED on discovery board to provide visual indication of what user-defined task is executing
	STM_EVAL_LEDOn(green_led);

	// execute an empty loop for duration of execution time
	while ( end_time > xTaskGetTickCount())
	{
		// loop for TASK_2_EXECUTION_TIME
	}

	// turn off LED here
	STM_EVAL_LEDOff(green_led);

	// execution time is done, delete task
	delete_dd_task(2);

	// ends its task
	vTaskDelete(NULL);
}

static void User_Task_3( void *pvParameters )
{
	TickType_t start_time = xTaskGetTickCount();
	TickType_t end_time = start_time + TASK_3_EXECUTION_TIME / portTICK_PERIOD_MS;

	// could turn on LED on discovery board to provide visual indication of what user-defined task is executing
	STM_EVAL_LEDOn(blue_led);

	// execute an empty loop for duration of execution time
	while ( end_time > xTaskGetTickCount())
	{
		// loop for TASK_3_EXECUTION_TIME
	}

	// turn off LED here
	STM_EVAL_LEDOff(blue_led);

	// execution time is done, delete task
	delete_dd_task(3);

	// ends its task
	vTaskDelete(NULL);
}

static void Monitor_Task( void *pvParameters )
{
	while(1)
	{
		// periodically reports processor utilization
		vTaskDelay(1000); // probably need to change delay later to appropriate amount

		// call get_active_dd_task_list
		int active_list_count = print_count_of_list (get_active_dd_task_list());
		// call get_complete_dd_task_list
		int complete_list_count = print_count_of_list(get_complete_dd_task_list());
		// call get_overdue_dd_task_list
		//int overdue_list_count = print_count_of_list(get_overdue_dd_task_list());

		// print to console number of tasks in each list
		printf("In Monitoring Task\n");
		printf("Active task list count: %d\n", active_list_count);
		printf("Complete task list count: %d\n", complete_list_count);
		//printf("Overdue task list count: %d\n", overdue_list_count);

		// additional challenge: measure and report processor utilization and system overhead (to do this check status and avilability of CPU using FreeRTOS APIs)
		//vTaskGetRunTimeStats( char *pcWriteBuffer ); // this task 
	}
}

/*------------------ helper functions -----------------------------------------*/
void create_dd_task( TaskHandle_t t_handle, enum task_type type, uint32_t the_task_id, uint32_t absolute_deadline)
{
	// receive information needed to create new dd_task struct (minus release time and completion time)
	/*struct dd_task *this_task = (struct dd_task *)pvPortMalloc(sizeof(struct dd_task *));
	this_task->t_handle = t_handle;
	this_task->type = type;
	this_task->task_id = task_id;
	this_task->release_time = 0;
	this_task->absolute_deadline = absolute_deadline;
	this_task->completion_time = 0;*/

	// struct is packaged as message and
	message* this_message = (message *)pvPortMalloc(sizeof(message *));
	this_message->task_info = (struct dd_task *)pvPortMalloc(sizeof(struct dd_task *));
	this_message->type = (enum message_type)pvPortMalloc(sizeof(enum message_type));

	this_message->type = create_dd;
	this_message->task_info->t_handle = t_handle;
	this_message->task_info->type = type;
	this_message->task_info->task_id = the_task_id;
	this_message->task_info->release_time = 0;
	this_message->task_info->absolute_deadline = absolute_deadline;
	this_message->task_info->completion_time = 0;
	this_message->task_id = the_task_id;

	// sends message struct  to queue (for DDS to receive)
	if (xQueueSend(xQueue_request_handle, &this_message, 1000))
	{
		// wait for reply from DDS (obtain reply message)

	}

	//vPortFree(this_message->task_info);
	vPortFree(this_message);
	//vPortFree(this_message->type);
}

void delete_dd_task(uint32_t task_id)
{
	// receive ID of DD-Task when it has completed its execution (from user task)
	// struct with id is packaged as message
	message* this_message = (message *)pvPortMalloc(sizeof(message *));
	this_message->type = (enum message_type)pvPortMalloc(sizeof(enum message_type));
	this_message->type = delete_dd;
	this_message->task_id = task_id;

	//sends to queue (for DDS to receive)
	if (xQueueSend(xQueue_request_handle, &this_message, 1000))
	{
		// wait for reply from DDS (obtain reply message)

	}

	//vPortFree(this_message);
}

struct dd_task_list* get_active_dd_task_list(void)
{
	// sends message to queue requesting Active Task list from DDS
	message* this_message = (message *)pvPortMalloc(sizeof(message *));
	//this_message->task_info = (struct dd_task *)pvPortMalloc(sizeof(struct dd_task *));
	//this_message->type = (enum message_type)pvPortMalloc(sizeof(enum message_type));
	this_message->type = get_active_dd_list;

	//sends to queue (for DDS to receive)
	if (xQueueSend(xQueue_request_handle, &this_message, 1000))
	{
		// wait for reply from DDS (obtain reply message)

	}

	vPortFree(this_message);

	// wait for reply from DDS (obtain reply message)
	message* reply_message = (message *)pvPortMalloc(sizeof(message *));
	// when response is received from DDS, function returns the list
	if (xQueueReceive(xQueue_response_handle, &reply_message, portMAX_DELAY))
	{
		// check for message type
		if (reply_message->type == get_active_dd_list)
		{
			return reply_message->task_list;
		}
		else {
			// if wrong message type, put back on queue
			if (xQueueSend(xQueue_response_handle, &reply_message, 1000))
			{
				// wait for reply from DDS (obtain reply message)

			}
		}
	}

	//vPortFree(this_message);

	return reply_message->task_list;
}

struct dd_task_list* get_complete_dd_task_list(void)
{
	// sends message to queue requesting Complete Task list from DDS
	message* this_message = (message *)pvPortMalloc(sizeof(message *));
	this_message->type = get_completed_dd_list;

	//sends to queue (for DDS to receive)
	if (xQueueSend(xQueue_request_handle, &this_message, 1000))
	{
		// wait for reply from DDS (obtain reply message)

	}

	//vPortFree(this_message);

	// wait for reply from DDS (obtain reply message)
	message* reply_message = (message *)pvPortMalloc(sizeof(message *));
	// when response is received from DDS, function returns the list
	if (xQueueReceive(xQueue_response_handle, &reply_message, portMAX_DELAY))
	{
		// check for message type
		if (reply_message->type == get_completed_dd_list)
		{
			//vPortFree(this_message);
			return reply_message->task_list;
		}
		else {
			// if wrong message type, put back on queue
			if (xQueueSend(xQueue_response_handle, &reply_message, 1000))
			{
				// wait for reply from DDS (obtain reply message)

			}
		}
	}

	//vPortFree(this_message);

	return reply_message->task_list;
}

struct dd_task_list* get_overdue_dd_task_list(void)
{
	// sends message to queue requesting Overdue Task list from DDS
	message* this_message = (message *)pvPortMalloc(sizeof(message *));
	this_message->type = get_overdue_dd_list;

	// sends to queue (for DDS to receive)
	if (xQueueSend(xQueue_request_handle, &this_message, 1000))
	{
		// sent successfully
	}

	//vPortFree(this_message);

	// wait for reply from DDS (obtain reply message)
	message* reply_message = (message *)pvPortMalloc(sizeof(message *));
	// when response is received from DDS, function returns the list
	if (xQueueReceive(xQueue_response_handle, &reply_message, portMAX_DELAY))
	{
		// check for message type
		if (reply_message->type == get_overdue_dd_list)
		{
			return reply_message->task_list;
		}
		else {
			// if wrong message type, put back on queue
			if (xQueueSend(xQueue_response_handle, &reply_message, 1000))
			{
				// wait for reply from DDS (obtain reply message)

			}
		}
	}

	//vPortFree(this_message);

	return reply_message->task_list;
}

/* linked list functions */
// add new task
dd_task_list * add_task( dd_task_list * list_head, struct dd_task * new_dd_task )
{
	/*dd_task_list *temp = (dd_task_list *)pvPortMalloc(sizeof(dd_task_list *));
	struct dd_task_list *p;

	if (list_head == NULL)
	{
		list_head = (dd_task_list *)pvPortMalloc(sizeof(dd_task_list *));
		list_head->task = *new_dd_task;
		list_head->next_task = NULL;
	}
	else {
		p = list_head;

		temp->task = *new_dd_task;
		temp->next_task = NULL;

		while (p->next_task != NULL)
		{
			p = p->next_task;
		}
		p->next_task = temp;
	}

	vPortFree(temp);*/
	dd_task_list* new_node = (dd_task_list *)pvPortMalloc(sizeof(dd_task_list *));
	//new_node->task = (struct dd_task *)pvPortMalloc(sizeof(struct dd_task *));
	new_node->task = *new_dd_task;
	new_node->next_task = list_head;

	return new_node;
}

// delete task
dd_task_list * delete_task ( struct dd_task_list * list_head, struct dd_task * done_dd_task )
{
	// Store head node
    struct dd_task_list *temp = list_head;
	struct dd_task_list *prev = NULL;

    // If head node itself holds the key to be deleted
    if (temp != NULL && temp->task.t_handle == done_dd_task->t_handle) {
        list_head = temp->next_task; // Changed head
        //free(temp); // free old head
        //return;
    }

    // Search for the task to be deleted, keep track of the
    // previous node as we need to change 'prev->next'
    while (temp != NULL && temp->task.t_handle != done_dd_task->t_handle) {
        prev = temp;
        temp = temp->next_task;
    }

    // If key was not present in linked list
    /*if (temp == NULL) {
    	return list_head;
    }*/

    // Unlink the node from linked list
    prev->next_task = temp->next_task;

    //vPortFree(temp); // Free memory

    return list_head;
}

// return count of list
int print_count_of_list ( struct dd_task_list * dd_task_list_head )
{
	struct dd_task_list *p = dd_task_list_head;
	int count = 0;
	while(p != NULL){
	    p = p->next_task;
	    count++;
	}

	return count;
}

// reorder by deadline first
dd_task_list * order_tasks_deadline_first( dd_task_list * dd_task_list_head )
{
	struct dd_task_list *current = dd_task_list_head;
	struct dd_task_list *index = NULL;
	struct dd_task temp;

	if (dd_task_list_head == NULL){
		return dd_task_list_head;
	}
	else {
		while (current != NULL){
			index = current->next_task;

			while (index != NULL){
				if (current->task.absolute_deadline < index->task.absolute_deadline){
					temp = current->task;
					current->task = index->task;
					index->task = temp;
				}
				index = index->next_task;
			}
			current = current->next_task;
		}
	}

	//vPortFree(index);

	return dd_task_list_head;
}

// assign task priorities
dd_task_list * assign_task_priorities ( dd_task_list * dd_task_list_head )
{
	int priority = 3; // start priority

	if (dd_task_list_head == NULL){
		return dd_task_list_head;
	}
	else {
		struct dd_task_list *temp; // = (dd_task_list *)pvPortMalloc(sizeof(dd_task_list *));
		temp = dd_task_list_head->next_task;
		vTaskPrioritySet(dd_task_list_head->task.t_handle, 3); // highest priority without being in way of monitor task
		if (priority - 1 != 0)
		{
			priority--; // can't make priority lower than one
		}

		// set the rest of tasks priorities
		while(temp != NULL){
			vTaskPrioritySet(temp->task.t_handle, priority); // each task is 1 level priority lower than prev task (should they go from 3 to 1 or is 2 and 1 okay?)
			temp = temp->next_task;
			if (priority - 1 != 0)
			{
				priority--; // can't make priority lower than one
			}
		}

		//vPortFree(temp);
	}

	return dd_task_list_head;
}

struct dd_task get_dd_task(struct dd_task_list * list_head, uint32_t the_task_id)
{
	// Store head node
	struct dd_task_list *current = list_head;

	// If head node itself holds the key to be deleted
	if (current != NULL && current->task.task_id == the_task_id) {
		current = list_head; // Changed head
	}

	// Search for the earliest task with matching task id
	while (current != NULL && current->task.task_id != the_task_id) {
		current = current->next_task;
	}

	return current->task;
}

/*------------------ software timer callbacks -----------------------------------------*/
void vGenerator1CallbackFunction( TimerHandle_t xGenerator1Timer )
{
	// this function gets called when task 1 period complete --> need to call task generator to create new user task 1
	vTaskResume(xGenerator1TaskHandle);
}

void vGenerator2CallbackFunction( TimerHandle_t xGenerator2Timer )
{
	// this function gets called when task 1 period complete --> need to call task generator to create new user task 1
	vTaskResume(xGenerator2TaskHandle);
}

void vGenerator3CallbackFunction( TimerHandle_t xGenerator3Timer )
{
	// this function gets called when task 1 period complete --> need to call task generator to create new user task 1
	vTaskResume(xGenerator3TaskHandle);
}

// could add another timer callback for overdue tasks (would be started each time a task is created?)

/*-----------------------------------------------------------*/

void vApplicationMallocFailedHook( void )
{
	/* The malloc failed hook is enabled by setting
	configUSE_MALLOC_FAILED_HOOK to 1 in FreeRTOSConfig.h.

	Called if a call to pvPortMalloc() fails because there is insufficient
	free memory available in the FreeRTOS heap.  pvPortMalloc() is called
	internally by FreeRTOS API functions that create tasks, queues, software 
	timers, and semaphores.  The size of the FreeRTOS heap is set by the
	configTOTAL_HEAP_SIZE configuration constant in FreeRTOSConfig.h. */
	for( ;; );
}
/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook( xTaskHandle pxTask, signed char *pcTaskName )
{
	( void ) pcTaskName;
	( void ) pxTask;

	/* Run time stack overflow checking is performed if
	configconfigCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
	function is called if a stack overflow is detected.  pxCurrentTCB can be
	inspected in the debugger if the task name passed into this function is
	corrupt. */
	for( ;; );
}
/*-----------------------------------------------------------*/

void vApplicationIdleHook( void )
{
	volatile size_t xFreeStackSpace;

	/* The idle task hook is enabled by setting configUSE_IDLE_HOOK to 1 in
	FreeRTOSConfig.h.

	This function is called on each cycle of the idle task.  In this case it
	does nothing useful, other than report the amount of FreeRTOS heap that
	remains unallocated. */
	xFreeStackSpace = xPortGetFreeHeapSize();

	if( xFreeStackSpace > 100 )
	{
		/* By now, the kernel has allocated everything it is going to, so
		if there is a lot of heap remaining unallocated then
		the value of configTOTAL_HEAP_SIZE in FreeRTOSConfig.h can be
		reduced accordingly. */
	}
}
/*-----------------------------------------------------------*/

static void prvSetupHardware( void )
{
	/* Ensure all priority bits are assigned as preemption priority bits.
	http://www.freertos.org/RTOS-Cortex-M3-M4.html */
	NVIC_SetPriorityGrouping( 0 );

	/* TODO: Setup the clocks, etc. here, if they were not configured before
	main() was called. */
}

