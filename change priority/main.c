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

/* The two task functions. */
void vTask1( void *pvParameters );
void vTask2( void *pvParameters );

/* Used to hold the handle of Task2. */
TaskHandle_t xTask2Handle;

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
    xTaskCreate( vTask1, "Task 1", 100, NULL, 2, NULL );
    xTaskCreate( vTask2, "Task 2", 100, NULL, 1, &xTask2Handle );
	  vTaskStartScheduler();	
	 for( ;; );
	return 0;

}
void vTask1( void *pvParameters )
{
UBaseType_t uxPriority;

	uxPriority = uxTaskPriorityGet( NULL );
	printf("priority %d\t",uxPriority);

	for( ;; )
	{
		/* Print out the name of this task. */
		printf( "Task1 is running\r\n" );

		/* Setting the Task2 priority above the Task1 priority will cause
		Task2 to immediately start running (as then Task2 will have the higher 
		priority of the    two created tasks). */
		printf("Task1->priority %d\t",uxPriority);
		printf( "About to raise the Task2 priority\r\n" );
		vTaskPrioritySet( xTask2Handle, ( uxPriority + 1 ) );

		/* Task1 will only run when it has a priority higher than Task2.
		Therefore, for this task to reach this point Task2 must already have
		executed and set its priority back down to 0. */
	}
}

/*-----------------------------------------------------------*/

void vTask2( void *pvParameters )
{
UBaseType_t uxPriority;

	/* Task1 will always run before this task as Task1 has the higher priority.
	Neither Task1 nor Task2 ever block so will always be in either the
	Running or the Ready state.

	Query the priority at which this task is running - passing in NULL means
	"return our own priority". */
	uxPriority = uxTaskPriorityGet( NULL );

	for( ;; )
	{
		/* For this task to reach this point Task1 must have already run and
		set the priority of this task higher than its own.

		Print out the name of this task. */
		printf( "Task2 is running\r\n" );
		printf("TAsk2->priority %d\t",uxPriority);
		/* Set our priority back down to its original value.  Passing in NULL
		as the task handle means "change our own priority".  Setting the
		priority below that of Task1 will cause Task1 to immediately start
		running again. */
		printf( "About to lower the Task2 priority\r\n" );
		vTaskPrioritySet( NULL, ( uxPriority - 2 ) );
		
	}
}
/*-----------------------------------------------------------*/





