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

#define mainBACKLIGHT_TIMER_PERIOD		( pdMS_TO_TICKS( 5000UL ) )
static void prvBacklightTimerCallback( TimerHandle_t xTimer );
static void vKeyHitTask( void *pvParameters );
static BaseType_t xSimulatedBacklightOn = pdFALSE;
static TimerHandle_t xBacklightTimer = NULL ;

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
	 xSimulatedBacklightOn = pdFALSE;

	/* Create the one shot timer, storing the handle to the created timer in
	xOneShotTimer. */
	xBacklightTimer = xTimerCreate( "Backlight",				/* Text name for the timer - not used by FreeRTOS. */
									mainBACKLIGHT_TIMER_PERIOD,	/* The timer's period in ticks. */
									pdFALSE,					/* Set uxAutoRealod to pdFALSE to create a one-shot timer. */
									0,							/* The timer ID is not used in this example. */
									prvBacklightTimerCallback );/* The callback function to be used by the timer being created. */

	/* A real application, running on a real target, would probably read button
	pushes in an interrupt.  That allows the application to be event driven, and
	prevents CPU time being wasted by polling for key presses when no keys have
	been pressed.  It is not practical to use real interrupts when using the
	FreeRTOS Windows port, so the vKeyHitTask() task is created to instead 
	provide the	key reading functionality by simply polling the keyboard. */
	xTaskCreate( vKeyHitTask, "Key poll", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL );

	/* Start the timer. */
	xTimerStart( xBacklightTimer, 0 );

	/* Start the scheduler. */
	vTaskStartScheduler();

	/* As in previous examples, vTaskStartScheduler() shoudl not return so the
	following lines should not be reached. */
	for( ;; );
	return 0;

 
}

static void vKeyHitTask( void *pvParameters )
{
const TickType_t xShortDelay = pdMS_TO_TICKS( 50 ); BaseType_t xKeyPressesStopApplication;
TickType_t xTimeNow;

	/* This example uses key presses, so prevent key presses being used to end
	the application. */
	xKeyPressesStopApplication = pdFALSE;

	printf( "Press a key to turn the backlight on.\r\n" );

	/* A real application, running on a real target, would probably read button
	pushes in an interrupt.  That allows the application to be event driven, and
	prevents CPU time being wasted by polling for key presses when no keys have
	been pressed.  It is not practical to use real interrupts when using the
	FreeRTOS Windows port, so this task is created to instead provide the key 
	reading functionality by simply polling the keyboard. */
	for( ;; )
	{
		/* Has a key been pressed? */
		
		//if( _kbhit() != 0 )
		{
			/* Record the time at which the key press was noted. */
			xTimeNow = xTaskGetTickCount();

			/* A key has been pressed. */
			if( xSimulatedBacklightOn == pdFALSE )
			{
				/* The backlight was off so turn it on and print the time at
				which it was turned on. */
				xSimulatedBacklightOn = pdTRUE;
			 printf( "Key pressed, turning backlight ON at time\t-> %d\r\n", xTimeNow );
			}
			else
			{
				/* The backlight was already on so print a message to say the
				backlight is about to be reset and the time at which it was
				reset. */
				 printf( "Key pressed, resetting software timer at time\t-> %d\r\n", xTimeNow );
			}

			/* Reset the software timer.  If the backlight was previously off
			this call will start the timer.  If the backlight was previously on
			this call will restart the timer.  A real application will probably
			read key presses in an interrupt.  If this function was an interrupt
			service routine then xTimerResetFromISR() must be used instead of
			xTimerReset(). */
			xTimerReset( xBacklightTimer, xShortDelay );

			/* Read and discard the key that was pressed. */
			//( void ) _getch();
		}

		/* Don't poll too quickly. */
		vTaskDelay( xShortDelay );
	}
}
/*-----------------------------------------------------------*/
static void prvBacklightTimerCallback( TimerHandle_t xTimer )
{
TickType_t xTimeNow = xTaskGetTickCount();

	/* The backlight timer expired, turn the backlight off. */
	xSimulatedBacklightOn = pdFALSE;

	/* Print the time at which the backlight was turned off. */
	printf( "Timer expired, turning backlight OFF at time\t-> %d\r\n", xTimeNow );
}
/*-----------------------------------------------------------*/

