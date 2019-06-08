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

void vTask1(void *pvParameters);
void vTask2(void *pvParameters);

TaskHandle_t xTask2Handle;

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
	  xTaskCreate( vTask1, "Task 1", 100, NULL, 1, NULL );
	  vTaskStartScheduler();	
	  for( ;; );
	   return 0;
}

void vTask1( void *pvParameters )
{
const TickType_t xDelay100ms = pdMS_TO_TICKS( 100UL );

	for( ;; )
	{
		/* Print out the name of this task. */
		printf( "Task1 is running\r\n" );

		/* Create task 2 at a higher priority.  Again the task parameter is not 
		used so is set to NULL - BUT this time we want to obtain a handle to the 
		task so pass in the address of the xTask2Handle variable. */
		xTaskCreate( vTask2, "Task 2", 100, NULL, 2, &xTask2Handle );
			 /* The task handle is the last parameter ^^^^^^^^^^^^^ */

		/* Task2 has/had the higher priority, so for Task1 to reach here Task2
		must have already executed and deleted itself.  Delay for 100 
		milliseconds. */
		vTaskDelay( xDelay100ms );
	}
}

void vTask2( void *pvParameters )
{
	/* Task2 does nothing but delete itself.  To do this it could call vTaskDelete()
	using a NULL parameter, but instead and purely for demonstration purposes it
	instead calls vTaskDelete() with its own task handle. */
	printf( "Task2 is running and about to delete itself\r\n" );
	vTaskDelete( xTask2Handle );
}
