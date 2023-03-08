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

#define red 1
#define yellow 2
#define green 3
/*
#define amber  	0
#define green  	1
#define red  	2
#define blue  	3

#define amber_led	LED3
#define green_led	LED4
#define red_led		LED5
#define blue_led	LED6

#define REG_CLOCK GPIO_Pin_7
#define REG_RESET GPIO_Pin_8
#define REG_DATA GPIO_Pin_6

#define RED_LIGHT GPIO_Pin_0
#define YELLOW_LIGHT GPIO_Pin_1
#define GREEN_LIGHT GPIO_Pin_2

#define POT_INPUT GPIO_Pin_3*/

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

// task generator for DD_Task_1 (will have one for each DD task)
static void DD_Task_Generator_1( void *pvParameters )
{
	// normal state: suspended, resumed when software timer callback is triggered
	// should reuse F-Task handles inside each DD task
	while(1)
	{
		// periodically generate user tasks for DD_Scheduler
		// make generated task periodic or aperiodic
		// prepares all information needed for creating specific instance of DD-tasks
		// calls release_dd_task
	}
}

static void User_Task_1( void *pvParameters )
{
	// cant rely on any communication with other tasks to complete its execution
	// execute an empty loop for duration of execution time
	// could use LED on discovery board to provide visual indication of what user-defined task is executing
	// calls complete_dd_task when finished executing
	for ( ; ; )
	{

	}

	create_dd_task();
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
	// struct is packaged as message and sends to queue (for DDS to receive)
	// wait for reply from DDS (obtain reply message)
}

void delete_dd_task(uint32_t task_id)
{
	// receive ID of DD-Task when it has completed its execution (from where?)
	// packages ID as message and sends to queue (for DDS to receive)
}

struct dd_task_list** get_active_dd_task_list(void)
{
	// sends message to queue requesting Active Task list from DDS
	// when response is received from DDS, function returns the list
}

struct dd_task_list** get_complete_dd_task_list(void)
{
	// sends message to queue requesting Complete Task list from DDS
	// when response is received from DDS, function returns the list
}

struct dd_task_list** get_overdue_dd_task_list(void)
{
	// sends message to queue requesting Overdue Task list from DDS
	// when response is received from DDS, function returns the list
}

/* linked list functions */
// add new task
void add_task( struct dd_task_list list_head, struct dd_task new_dd_task )
{
	struct dd_task_list temp;
	temp->task = new_dd_task;

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
/*void vRedLightCallbackFunction( TimerHandle_t xTimer )
{
	// this function gets called when red light timer done --> switch to green light
	printf("RedLightCallback: Red light timer done");
	GPIO_ResetBits(GPIOC, RED_LIGHT);

	// need to update value in lights mutex
	if (xSemaphoreTake( xSemaphore_lights, (TickType_t) 0))
	{
		GPIO_SetBits(GPIOC, GREEN_LIGHT);
		light_status = green;
		printf("RedLightCallback: Light status updated to green");

		if (xSemaphoreGive(xSemaphore_lights) != pdTRUE)
		{
			printf("RedLightCallback: Giving semaphore failed");
		}
	}

	xTimerStart(xGreenLightTimer, 0);
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

