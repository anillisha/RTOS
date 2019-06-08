/*................................................................................
PROJECT TITLE: FREE RTOS TUTORIAL 
          MCU: NUVOTON CORTEX M-0, 
Starting Date: 2/2/2019
         By: Anil Kumar
	  mail Id: eceaa2@gmail.com
modificationDate: 5/2/2019
******************************************************************* */
#include <stdio.h>
#include "NuMicro.h"
#include "freertos_evr.h"
#include "cmsis_compiler.h"
#include "FreeRTOS.h"                   // ARM.FreeRTOS::RTOS:Core
#include "task.h"                       // ARM.FreeRTOS::RTOS:Core
#include "semphr.h"                     // ARM.FreeRTOS::RTOS:Core
#include "event_groups.h"               // ARM.FreeRTOS::RTOS:Event Groups
#include "stream_buffer.h"     

void vSenderTask1( void *pvParameters );
void vSenderTask2( void *pvParameters );
void vReceiverTask( void *pvParameters );

static QueueHandle_t xQueue1 = NULL, xQueue2 = NULL;
static QueueSetHandle_t xQueueSet = NULL;

int i;
void SYS_Init(void)
{
    /* Enable IP clock */
    CLK->APBCLK = CLK_APBCLK_UART0_EN_Msk;

    /* Update System Core Clock */
    /* User can use SystemCoreClockUpdate() to calculate SystemCoreClock and cyclesPerUs automatically. */
    SystemCoreClockUpdate();

    /* Set GPB multi-function pins for UART0 RXD and TXD */
    SYS->GPB_MFP &= ~(SYS_GPB_MFP_PB0_Msk | SYS_GPB_MFP_PB1_Msk);
    SYS->GPB_MFP |= SYS_GPB_MFP_PB0_UART0_RXD | SYS_GPB_MFP_PB1_UART0_TXD;
}


int main()
{
    int8_t ch;

    /* Unlock protected registers */
    SYS_UnlockReg();

    SYS_Init();

    /* Lock protected registers */
    SYS_LockReg();

    /* Init UART0 to 115200-8n1 for print message */
    UART_Open(UART0, 115200);
	   GPIO_SetMode(PB, BIT2, GPIO_PMD_OUTPUT);
	 

    printf("Simple Demo Code\n\n");
	  xQueue1 = xQueueCreate( 1, sizeof( char * ) );
	  xQueue2 = xQueueCreate( 1, sizeof( char * ) );
	 xQueueSet = xQueueCreateSet( 1 * 2 );

	/* Add the two queues to the set. */
	xQueueAddToSet( xQueue1, xQueueSet );
	xQueueAddToSet( xQueue2, xQueueSet );

	/* Create the tasks that send to the queues. */
	xTaskCreate( vSenderTask1, "Sender1", 100, NULL, 1, NULL );
	xTaskCreate( vSenderTask2, "Sender2", 100, NULL, 1, NULL );

	/* Create the receiver task. */
	xTaskCreate( vReceiverTask, "Receiver", 100, NULL, 2, NULL );

	/* Start the scheduler so the created tasks start executing. */
	vTaskStartScheduler();

	/* The following line should never be reached because vTaskStartScheduler()
	will only return if there was not enough FreeRTOS heap memory available to
	create the Idle and (if configured) Timer tasks.  Heap management, and
	techniques for trapping heap exhaustion, are described in the book text. */
	for( ;; );
	return 0;
 
}

static void vSenderTask1( void *pvParameters )
{
const TickType_t xBlockTime = pdMS_TO_TICKS( 100 );
const char * const pcMessage = "Message from vSenderTask1\r\n";

	/* As per most tasks, this task is implemented within an infinite loop. */
	for( ;; )
	{
		/* Block for 100ms. */
		vTaskDelay( xBlockTime );

		/* Send this task's string to xQueue1. It is not necessary to use a
		block time, even though the queue can only hold one item.  This is
		because the priority of the task that reads from the queue is higher
		than the priority of this task; as soon as this task writes to the queue
		it will be pre-empted by the task that reads from the queue, so the
		queue will already be empty again by the time the call to xQueueSend()
		returns.  The block time is set to 0. */
		xQueueSend( xQueue1, &pcMessage, 0 );
	}
}
/*-----------------------------------------------------------*/

static void vSenderTask2( void *pvParameters )
{
const TickType_t xBlockTime = pdMS_TO_TICKS( 200 );
const char * const pcMessage = "Message from vSenderTask2\r\n";

	/* As per most tasks, this task is implemented within an infinite loop. */
	for( ;; )
	{
		/* Block for 200ms. */
		vTaskDelay( xBlockTime );

		/* Send this task's string to xQueue1. It is not necessary to use a
		block time, even though the queue can only hold one item.  This is
		because the priority of the task that reads from the queue is higher
		than the priority of this task; as soon as this task writes to the queue
		it will be pre-empted by the task that reads from the queue, so the
		queue will already be empty again by the time the call to xQueueSend()
		returns.  The block time is set to 0. */
		xQueueSend( xQueue2, &pcMessage, 0 );
	}
}
/*-----------------------------------------------------------*/

static void vReceiverTask( void *pvParameters )
{
QueueHandle_t xQueueThatContainsData;
char *pcReceivedString;

	/* As per most tasks, this task is implemented within an infinite loop. */
	for( ;; )
	{
		/* Block on the queue set to wait for one of the queues in the set to
		contain data.  Cast the QueueSetMemberHandle_t values returned from
		xQueueSelectFromSet() to a QueueHandle_t as it is known that all the
		items in the set are queues (as opposed to semaphores, which can also be
		members of a queue set). */
		xQueueThatContainsData = ( QueueHandle_t ) xQueueSelectFromSet( xQueueSet, portMAX_DELAY );

		/* An indefinite block time was used when reading from the set so
		xQueueSelectFromSet() will not have returned unless one of the queues in
		the set contained data, and xQueueThatContansData must be valid.  Read
		from the queue.  It is not necessary to specify a block time because it
		is known that the queue contains data.  The block time is set to 0. */
		xQueueReceive( xQueueThatContainsData, &pcReceivedString, 0 );

		/* Print the string received from the queue. */
		printf( "%s",pcReceivedString );
	}
}