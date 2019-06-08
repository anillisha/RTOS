/*................................................................................
PROJECT TITLE: FREE RTOS TUTORIAL 
          MCU: NUVOTON CORTEX M-0, 
Starting Date: 5/2/2019
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

#define mainONE_SHOT_TIMER_PERIOD		( pdMS_TO_TICKS( 3333UL ) )
#define mainAUTO_RELOAD_TIMER_PERIOD	( pdMS_TO_TICKS( 500UL ) )

static void prvTimerCallback( TimerHandle_t xTimer );
static void prvTimerCallback( TimerHandle_t xTimer );
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

static TimerHandle_t xAutoReloadTimer, xOneShotTimer;
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
BaseType_t xTimer1Started, xTimer2Started;

	/* Create the one shot timer, storing the handle to the created timer in
	xOneShotTimer. */
	xOneShotTimer = xTimerCreate( "OneShot",					/* Text name for the timer - not used by FreeRTOS. */
								  mainONE_SHOT_TIMER_PERIOD,	/* The timer's period in ticks. */
								  pdFALSE,						/* Set uxAutoRealod to pdFALSE to create a one-shot timer. */
								  0,							/* The timer ID is initialised to 0. */
								  prvTimerCallback );			/* The callback function to be used by the timer being created. */

	/* Create the auto-reload, storing the handle to the created timer in
	xAutoReloadTimer. */
	xAutoReloadTimer = xTimerCreate( "AutoReload",					/* Text name for the timer - not used by FreeRTOS. */
									 mainAUTO_RELOAD_TIMER_PERIOD,	/* The timer's period in ticks. */
									 pdTRUE,						/* Set uxAutoRealod to pdTRUE to create an auto-reload timer. */
									 0,								/* The timer ID is initialised to 0. */
									 prvTimerCallback );			/* The callback function to be used by the timer being created. */

	/* Check the timers were created. */
	if( ( xOneShotTimer != NULL ) && ( xAutoReloadTimer != NULL ) )
	{
		/* Start the timers, using a block time of 0 (no block time).  The
		scheduler has not been started yet so any block time specified here
		would be ignored anyway. */
		xTimer1Started = xTimerStart( xOneShotTimer, 0 );
		xTimer2Started = xTimerStart( xAutoReloadTimer, 0 );

		/* The implementation of xTimerStart() uses the timer command queue, and
		xTimerStart() will fail if the timer command queue gets full.  The timer
		service task does not get created until the scheduler is started, so all
		commands sent to the command queue will stay in the queue until after
		the scheduler has been started.  Check both calls to xTimerStart()
		passed. */
		if( ( xTimer1Started == pdPASS ) && ( xTimer2Started == pdPASS ) )
		{
			/* Start the scheduler. */
			vTaskStartScheduler();
		}
	}

	/* If the scheduler was started then the following line should never be
	reached because vTaskStartScheduler() will only return if there was not
	enough FreeRTOS heap memory available to create the Idle and (if configured)
	Timer tasks.  Heap management, and techniques for trapping heap exhaustion,
	are described in the book text. */
	for( ;; );
	return 0;
 
}

static void prvTimerCallback( TimerHandle_t xTimer )
{
TickType_t xTimeNow;
uint32_t ulExecutionCount;

	/* The count of the number of times this software timer has expired is
	stored in the timer's ID.  Obtain the ID, increment it, then save it as the
	new ID value.  The ID is a void pointer, so is cast to a uint32_t. */
	ulExecutionCount = ( uint32_t ) pvTimerGetTimerID( xTimer );
	ulExecutionCount++;
	vTimerSetTimerID( xTimer, ( void * ) ulExecutionCount );

	/* Obtain the current tick count. */
	xTimeNow = xTaskGetTickCount();

    /* The handle of the one-shot timer was stored in xOneShotTimer when the
	timer was created.  Compare the handle passed into this function with
	xOneShotTimer to determine if it was the one-shot or auto-reload timer that
	expired, then output a string to show the time at which the callback was
	executed. */
	if( xTimer == xOneShotTimer )
	{
		printf( "One-shot timer callback executing %d\r\n", xTimeNow );
	}
	else
	{
        /* xTimer did not equal xOneShotTimer, so it must be the auto-reload
		timer that expired. */
		printf( "Auto-reload timer callback executing%d\r\n", xTimeNow );

		if( ulExecutionCount == 5 )
		{
			/* Stop the auto-reload timer after it has executed 5 times.  This
			callback function executes in the context of the RTOS daemon task so
			must not call any functions that might place the daemon task into
			the Blocked state.  Therefore a block time of 0 is used. */
			xTimerStop( xTimer, 0 );
		}
	}
}
