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

static void vSenderTask( void *pvParameters );
static void vReceiverTask( void *pvParameters );
QueueHandle_t xQueue;

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
	  xQueue = xQueueCreate( 5, sizeof( int32_t ) );
	  if( xQueue != NULL )
	  {
		   xTaskCreate( vSenderTask, "Sender1", 100, NULL, 1, NULL );
		   xTaskCreate( vSenderTask, "Sender2", 100, ( void * ) 200, 1, NULL );

		   xTaskCreate( vReceiverTask, "Receiver", 100, NULL, 2, NULL );
		   vTaskStartScheduler();
	}
	else
	{
		printf("QUEUE IS NOT CREATE\r\n");
	}
	for( ;; );
	return 0;

 
}

static void vSenderTask( void *pvParameters )
{
int32_t lValueToSend;
BaseType_t xStatus;

	lValueToSend = ( int32_t ) pvParameters;

	/* As per most tasks, this task is implemented within an infinite loop. */
	for( ;; )
	{
		xStatus = xQueueSendToBack( xQueue, &lValueToSend, 0 );

		if( xStatus != pdPASS )
		{
			printf( "Could not send to the queue.\r\n" );
		}
	}
}
/*-----------------------------------------------------------*/

static void vReceiverTask( void *pvParameters )
{
/* Declare the variable that will hold the values received from the queue. */
int32_t lReceivedValue;
BaseType_t xStatus;
const TickType_t xTicksToWait = pdMS_TO_TICKS( 100UL );
	for( ;; )
	{
		if( uxQueueMessagesWaiting( xQueue ) != 0 )
		{
			printf( "Queue should have been empty!\r\n" );
		}

		xStatus = xQueueReceive( xQueue, &lReceivedValue, xTicksToWait );

		if( xStatus == pdPASS )
		{
			printf( "Received = %d \r\n", lReceivedValue );
		}
		else
		{
			printf( "Could not receive from the queue.\r\n" );
		}
	}
}