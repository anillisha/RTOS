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


/* The task functions. */
void vContinuousProcessingTask( void *pvParameters );
void vPeriodicTask( void *pvParameters );

const char *pcTextForTask1 = "Continuous task 1 running\r\n";
const char *pcTextForTask2 = "Continuous task 2 running\r\n";
const char *pcTextForPeriodicTask = "Periodic task is running\r\n";

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

 


	xTaskCreate( vContinuousProcessingTask, "Task 1", 50, (void*)pcTextForTask1, 1, NULL );
	xTaskCreate( vContinuousProcessingTask, "Task 2", 50, (void*)pcTextForTask2, 1, NULL );

	/* Create one instance of the periodic task at priority 2. */
	xTaskCreate( vPeriodicTask, "Task 3", 50, (void*)pcTextForPeriodicTask, 2, NULL );

	/* Start the scheduler to start the tasks executing. */
	vTaskStartScheduler();

	/* The following line should never be reached because vTaskStartScheduler()
	will only return if there was not enough FreeRTOS heap memory available to
	create the Idle and (if configured) Timer tasks.  Heap management, and
	techniques for trapping heap exhaustion, are described in the book text. */
	for( ;; );
	return 0;
}
/*-----------------------------------------------------------*/

void vContinuousProcessingTask( void *pvParameters )
{
char *pcTaskName;

	/* The string to print out is passed in via the parameter.  Cast this to a
	character pointer. */
	pcTaskName = ( char * ) pvParameters;

	/* As per most tasks, this task is implemented in an infinite loop. */
	for( ;; )
	{
		/* Print out the name of this task.  This task just does this repeatedly
		without ever blocking or delaying. */
		printf("%s",pcTaskName );
	}
}
/*-----------------------------------------------------------*/

void vPeriodicTask( void *pvParameters )
{
TickType_t xLastWakeTime;
const TickType_t xDelay5ms = pdMS_TO_TICKS( 3UL );

	/* The xLastWakeTime variable needs to be initialized with the current tick
	count.  Note that this is the only time we access this variable.  From this
	point on xLastWakeTime is managed automatically by the vTaskDelayUntil()
	API function. */
	xLastWakeTime = xTaskGetTickCount();

	/* As per most tasks, this task is implemented in an infinite loop. */
	for( ;; )
	{
		/* Print out the name of this task. */
		printf( "Periodic task is running\r\n" );

		/* We want this task to execute exactly every 10 milliseconds. */
		vTaskDelayUntil( &xLastWakeTime, xDelay5ms );
	}
}