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
#include <stdlib.h>
#include "stm32f4_discovery.h"
/* Kernel includes. */
#include "stm32f4xx.h"
#include "../FreeRTOS_Source/include/FreeRTOS.h"
#include "../FreeRTOS_Source/include/queue.h"
#include "../FreeRTOS_Source/include/semphr.h"
#include "../FreeRTOS_Source/include/task.h"
#include "../FreeRTOS_Source/include/timers.h"

/*-----------------------------------------------------------*/
//#define mainQUEUE_LENGTH 100
#define QUEUE_LENGTH 100

/*
#define amber_led	LED3
#define green_led	LED4
#define red_led		LED5
#define blue_led	LED6
*/

/* test bench 1 */
#define TASK_1_EXECUTION_TIME 95
#define TASK_1_PERIOD 500
#define TASK_2_EXECUTION_TIME 150
#define TASK_1_PERIOD 500
#define TASK_3_EXECUTION_TIME 250
#define TASK_1_PERIOD 750

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

struct dd_task_list {
	struct dd_task task;
	struct dd_task_list *next_task;
};

enum message_type {release_dd_task, complete_dd_task, get_active_dd_task_list, get_completed_dd_task_list, get_overdue_dd_task_list}

struct message {
	enum message_type;
	struct dd_task task_info;
	uint32_t task_id;
}

/* helper functions */
void create_dd_task( TaskHandle_t t_handle, enum task_type type, uint32_t task_id, uint32_t absolute_deadline);
void delete_dd_task(uint32_t task_id);
struct dd_task_list ** get_active_dd_task_list(void);
struct dd_task_list ** get_complete_dd_task_list(void);
struct dd_task_list ** get_overdue_dd_task_list(void);

/* create linked lists */
struct dd_task_list *active_list;

/* linked list functions */


xQueueHandle xQueue_handle = 0;

/* tasks */
static void DD_Scheduler_Task( void *pvParameters );
static void DD_Task_Generator_1( void *pvParameters );
// static void DD_Task_Generator_2( void *pvParameters );
// static void DD_Task_Generator_3( void *pvParameters );
static void User_Task_1( void *pvParameters );
static void Monitor_Task( void *pvParameters );

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
	// setup GPIO
	// initialize traffic light GPIOs
	/* Configure the system ready to run the demo.  The clock configuration
		can be done here if it was not done before main() was called. */
	prvSetupHardware();

	// create queue to send and receive potentiometer value
	xQueue_request_handle = xQueueCreate( QUEUE_LENGTH, sizeof( uint16_t ));
	xQueue_response_handle = xQueueCreate( QUEUE_LENGTH, sizeof( uint16_t ));

	// add potentiometer queue to registry for debugging
	vQueueAddToRegistry( xQueue_request_handle, "RequestQueue" );
	vQueueAddToRegistry( xQueue_response_handle, "ResponseQueue" );

	/* Create the queue used by the queue send and queue receive tasks.
	http://www.freertos.org/a00116.html */
	//xQueue_handle = xQueueCreate( 	mainQUEUE_LENGTH,		/* The number of items the queue can hold. */
							//sizeof( uint16_t ) );	/* The size of each item the queue holds. */

	// need to decide priority and stack size
	xTaskCreate(DD_Scheduler_Task, "DD Scheduler", configMINIMAL_STACK_SIZE, NULL, 3, NULL);
	xTaskCreate(DD_Task_Generator_1, "DD Task Generator 1", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
	xTaskCreate(User_Task_1, "User Task", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
	xTaskCreate(Monitor_Task, "Monitor", configMINIMAL_STACK_SIZE, NULL, 1, NULL);

	// create software timers for controlling traffic lights
	xRedLightTimer = xTimerCreate("Generator 1", TASK_1_PERIOD / portTICK_PERIOD_MS, pdTRUE, (void *) 0, vGenerator1CallbackFunction);
	xYellowLightTimer = xTimerCreate("Generator 2", TASK_2_PERIOD  / portTICK_PERIOD_MS, pdTRUE, (void *) 0, vGenerator2CallbackFunction);
	xGreenLightTimer = xTimerCreate("Generator 3", TASK_3_PERIOD / portTICK_PERIOD_MS, pdTRUE, (void *) 0, vGenerator3CallbackFunction);

	/* Start the tasks and timer running. */
	vTaskStartScheduler();

	return 0;
}


/*------------Tasks-----------------------------------------------*/
static void DD_Scheduler_Task( void *pvParameters )
{
	// waits for scheduling task (usually should be suspended until scheduling task exists)
	// idk if this is correct for while loop
	//while(xQueueReceive(xQueue_request_handle, &message, 500))
	while(1)
	{
		// adjusts user task priorities  (Set earliest deadline task priority to high and the rest to low so first completes that and then so on)

		/*char[] message;
		// cases based on what message type is received
		if (message == "release_dd_task")
		{
			// assign release time to new task
			// add DD task to active task list
			// sort list by deadline
			// set priorities of user defined task according to sorted list
			// MAYBE: start software timer, callback function will be to check if overdue time is passed, when callback is called: send new message type (overdue_dd_task)
		}
		else if (message == "complete_dd_task"")
		{
			// assign completion time to newly completed DD-task
			// remove DD task from active task list
			// add DD task to complete task list
			// re-sort active task list by deadline
			// set priorities of user defined tasks accordingly
		}
		/* else if (message = overdue_dd_task)
		 * {
		 * 		//from call back function
		 * 		// remove DD task from active task list
		 * 		// add DD task to overdue task list
		 * 		// re-sort overdue task list by deadline
		 * 		// set priorities of user defined tasks accordingly
		 * }
		 */
		/*else if (message == "get_active_dd_task_list")
		{
			// get active task list and send to a queue
		}
		else if (message == "get_completed_dd_task_list"")
		{
			// get complete task list and send to a queue
		}
		else if (message == "get_overdue_dd_task_list")
		{
			// get overdue task list and send to a queue
		}*/

	}
}

// task generator for DD_User_Task_1 (will have one for each DD task)
static void DD_Task_Generator_1( void *pvParameters )
{
	// normal state: suspended, resumed when software timer callback is triggered, timer period would be TASK_1_PERIOD
	// should reuse F-Task handles inside each DD task
	// periodically generate user tasks for DD_Scheduler 
	xTaskCreate( User_Task_1, "User Task 1", configMINIMAL_STACK_SIZE, NULL, 1, &task_1_handle); // start it at lowest priority 
	vTaskSuspend(task_1_handle); // DD scheduler will start it when it should be run 

	// prepares all information needed for creating specific instance of DD-tasks
	// calls release_dd_task 
	// xTaskGetTickCount: The count of ticks since vTaskStartScheduler was called
	release_dd_task(task_1_handle, PERIODIC, 1,  xTaskGetTickCount( void ) + TASK_1_EXECUTION_TIME / portTICK_PERIOD_MS)

	// needs to suspend itself (NULL means suspends itself)
	vTaskSuspend( NULL );
}

// task generator for DD_User_Task_1 (will have one for each DD task)
static void DD_Task_Generator_2( void *pvParameters )
{
	// normal state: suspended, resumed when software timer callback is triggered, timer period would be TASK_2_PERIOD
	// should reuse F-Task handles inside each DD task
	// periodically generate user tasks for DD_Scheduler 
	xTaskCreate( User_Task_2, "User Task 2", configMINIMAL_STACK_SIZE, NULL, 1, &task_2_handle); // start it at lowest priority 
	vTaskSuspend(task_2_handle); // DD scheduler will start it when it should be run 

	// prepares all information needed for creating specific instance of DD-tasks
	// calls release_dd_task 
	// xTaskGetTickCount: The count of ticks since vTaskStartScheduler was called
	release_dd_task(task_2_handle, PERIODIC, 2,  xTaskGetTickCount( void ) + TASK_2_EXECUTION_TIME / portTICK_PERIOD_MS)

	// needs to suspend itself (NULL means suspends itself)
	vTaskSuspend( NULL );
}

// task generator for DD_User_Task_1 (will have one for each DD task)
static void DD_Task_Generator_3( void *pvParameters )
{
	// normal state: suspended, resumed when software timer callback is triggered, timer period would be TASK_3_PERIOD
	// should reuse F-Task handles inside each DD task
	// periodically generate user tasks for DD_Scheduler 
	xTaskCreate( User_Task_3, "User Task 3", configMINIMAL_STACK_SIZE, NULL, 1, &task_3_handle); // start it at lowest priority 
	vTaskSuspend(task_3_handle); // DD scheduler will start it when it should be run 

	// prepares all information needed for creating specific instance of DD-tasks
	// calls release_dd_task 
	// xTaskGetTickCount: The count of ticks since vTaskStartScheduler was called
	release_dd_task(task_3_handle, PERIODIC, 3,  xTaskGetTickCount( void ) + TASK_3_EXECUTION_TIME / portTICK_PERIOD_MS)

	// needs to suspend itself (NULL means suspends itself)
	vTaskSuspend( NULL );
}

static void User_Task_1( void *pvParameters )
{
	TickType_t start_time = xTaskGetTickCount( void );
	TickType_t end_time = start_time + TASK_1_EXECUTION_TIME / portTICK_PERIOD_MS;
	
	// could turn on LED on discovery board to provide visual indication of what user-defined task is executing

	// execute an empty loop for duration of execution time
	while ( end_time > xTaskGetTickCount( void ))
	{
		// loop for TASK_1_EXECUTION_TIME 
	}

	// turn off LED here 

	// execution time is done, delete task 
	delete_dd_task(1);
}

static void User_Task_2( void *pvParameters )
{
	TickType_t start_time = xTaskGetTickCount( void );
	TickType_t end_time = start_time + TASK_2_EXECUTION_TIME / portTICK_PERIOD_MS;
	
	// could turn on LED on discovery board to provide visual indication of what user-defined task is executing

	// execute an empty loop for duration of execution time
	while ( end_time > xTaskGetTickCount( void ))
	{
		// loop for TASK_2_EXECUTION_TIME 
	}

	// turn off LED here 

	// execution time is done, delete task 
	delete_dd_task(2);
}

static void User_Task_3( void *pvParameters )
{
	TickType_t start_time = xTaskGetTickCount( void );
	TickType_t end_time = start_time + TASK_3_EXECUTION_TIME / portTICK_PERIOD_MS;
	
	// could turn on LED on discovery board to provide visual indication of what user-defined task is executing

	// execute an empty loop for duration of execution time
	while ( end_time > xTaskGetTickCount( void ))
	{
		// loop for TASK_3_EXECUTION_TIME 
	}

	// turn off LED here 

	// execution time is done, delete task 
	delete_dd_task(3);
}

static void Monitor_Task( void *pvParameters )
{
	while(1)
	{
		// periodically reports processor utilization
		// scheduled by FreeRTOS
		// call get_active_dd_task_list
		// call get_complete_dd_task_list
		// call get_overdue_dd_task_list

		// print to console number of tasks in each list
		// additional challenge: measure and report processor utilization and system overhead (to do this check status and avilability of CPU using FreeRTOS APIs)
	}
}

/*------------------ helper functions -----------------------------------------*/
void create_dd_task( TaskHandle_t t_handle, enum task_type type, uint32_t task_id, uint32_t absolute_deadline)
{
	// receive information needed to create new dd_task struct (minus release time and completion time)
	struct dd_task this_task;
	this_task.t_handle = t_handle;
	this_task.type = type;
	this_task.task_id = task_id;
	this_task.release_time = NULL;
	this_task.absolute_deadline = absolute_deadline;
	this_task.completion_time = NULL;

	// struct is packaged as message and 
	struct message this_message;
	this_message.message_type = create_dd_task;
	this_message.task_info = this_task;

	// sends message struct  to queue (for DDS to receive)
	if (xQueueSend(xQueue_request_handle, &this_message, 1000))
	{
		// wait for reply from DDS (obtain reply message)
		
	}
}

void delete_dd_task(uint32_t task_id)
{
	// receive ID of DD-Task when it has completed its execution (from user task)
	// packages ID as message 
	// struct is packaged as message and 
	struct message this_message;
	this_message.message_type = delete_dd_task;
	this_message.task_id = task_id;

	//sends to queue (for DDS to receive)
	if (xQueueSend(xQueue_request_handle, &this_message, 1000))
	{
		// wait for reply from DDS (obtain reply message)
		
	}
}

struct dd_task_list** get_active_dd_task_list(void)
{
	// sends message to queue requesting Active Task list from DDS
	struct message this_message;
	this_message.message_type = get_active_dd_task_list;

	//sends to queue (for DDS to receive)
	if (xQueueSend(xQueue_request_handle, &this_message, 1000))
	{
		// wait for reply from DDS (obtain reply message)
		
	}

	// when response is received from DDS, function returns the list
}

struct dd_task_list** get_complete_dd_task_list(void)
{
	// sends message to queue requesting Complete Task list from DDS
	struct message this_message;
	this_message.message_type = get_complete_dd_task_list;

	//sends to queue (for DDS to receive)
	if (xQueueSend(xQueue_request_handle, &this_message, 1000))
	{
		// wait for reply from DDS (obtain reply message)
		
	}

	// when response is received from DDS, function returns the list
}

struct dd_task_list** get_overdue_dd_task_list(void)
{
	// sends message to queue requesting Overdue Task list from DDS
	struct message this_message;
	this_message.message_type = get_overdue_dd_task_list;

	//sends to queue (for DDS to receive)
	if (xQueueSend(xQueue_request_handle, &this_message, 1000))
	{
		// wait for reply from DDS (obtain reply message)
		
	}

	// when response is received from DDS, function returns the list
}

/* linked list functions */
// add new task
void add_task( struct dd_task_list list_head, struct dd_task new_dd_task )
{
	struct dd_task_list temp;
	temp.task = new_dd_task;

	struct dd_task_list p;

	if (list_head == NULL)
	{
		list_head = temp;
	}
	else {
		p = list_head;
		while (p->next_task != NULL)
		{
			p = p->next_task;
		}
		p->next_task = temp;
	}
}

// delete task
void delete_task ( TaskHandle_t t_handle )
{

}
// return full list
int return_count_of_list ( struct dd_task_list dd_task_list_head )
{
	dd_task_list p = dd_task_list_head;
	int count = 0;
	while(p != NULL){
	    p = p->next_task;
	    count++;
	}

	return count;
}

// reorder by deadline first
void order_tasks_deadline_first( struct dd_task_list dd_task_list_head )
{
	struct dd_task_list current = dd_task_list_head;
	struct dd_task_list *index = NULL;
	int temp;

	if (dd_task_list_head == NULL){
		return;
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
		}
		current = current->next_task;
	}
}

// reassign task priorities
void assign_task_priorities ( struct dd_task_list dd_task_list_head )
{

}

/*------------------ software timer callbacks -----------------------------------------*/
void vGenerator1CallbackFunction( TimerHandle_t xGenerator1Timer )
{
	// this function gets called when task 1 period complete --> need to call task generator to create new user task 1
	vTaskResume( /*task 1 generator task handle */);
}

void vGenerator2CallbackFunction( TimerHandle_t xGenerator2Timer )
{
	// this function gets called when task 1 period complete --> need to call task generator to create new user task 1
	vTaskResume( /*task 2 generator task handle */);
}

void vGenerator2CallbackFunction( TimerHandle_t xGenerator3Timer )
{
	// this function gets called when task 1 period complete --> need to call task generator to create new user task 1
	vTaskResume( /*task 3 generator task handle */);
}

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

