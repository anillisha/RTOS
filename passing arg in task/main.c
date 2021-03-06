#include <stdio.h>
#include "NuMicro.h"
#include "freertos_evr.h"
#include "cmsis_compiler.h"
#include "FreeRTOS.h"                   // ARM.FreeRTOS::RTOS:Core
#include "task.h"                       // ARM.FreeRTOS::RTOS:Core
#include "semphr.h"                     // ARM.FreeRTOS::RTOS:Core
#include "event_groups.h"               // ARM.FreeRTOS::RTOS:Event Groups
#include "stream_buffer.h"     

#define mainDELAY_LOOP_COUNT		( 0xffffff )

/* The task function. */
void vTaskFunction( void *pvParameters );
const char *pcTextForTask1 = "Task 1 is running\r\n";
const char *pcTextForTask2 = "Task 2 is running\r\n";
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
	  xTaskCreate(vTaskFunction,"TASK1",100,(void *)pcTextForTask1 ,1,NULL);
	  xTaskCreate(vTaskFunction,"TASK2",100,(void*)pcTextForTask2 , 1,NULL);   
	  vTaskStartScheduler();
	  for(;;);
	  return 0;

  
}

void vTaskFunction(void *pvParameters )
{
	unsigned int MS_delay;
  while(1)
	{
	  
			 printf(" i am in task help me\r\n");
		   printf(" I HELP TO YOu %s",pvParameters);
				for(MS_delay=0;MS_delay < 0xFFFF;MS_delay++)
				{
				 ;;
				}
  }

}