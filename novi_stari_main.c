#include <stdio.h>
#include "stm32f4xx.h"
#include "mb.h"

#include <main.h>

#include <stm32f4_discovery.h>

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

uint64_t u64Ticks=0;					// Counts OS ticks (default = 1000Hz)	
uint64_t u64IdleTicks=0;			// Value of u64IdleTicksCnt is copied once per sec
uint64_t u64IdleTicksCnt=0;		// Counts when the OS has no task to execute
// This FreeRTOS callback function gets called once per tick (default = 1000Hz)
// ----------------------------------------------------------------------------
void vApplicationTickHook( void ) { 
    ++u64Ticks; 
} 

// This FreeRTOS call-back function gets when no other task is ready to execute. 
// On a completely unloaded system this is getting called at over 2.5MHz! 
// ----------------------------------------------------------------------------  
void vApplicationIdleHook( void ) { 
    ++u64IdleTicksCnt; 
} 
// A required FreeRTOS function. 
// ----------------------------------------------------------------------------  
void vApplicationMallocFailedHook( void ) { 
    configASSERT( 0 );  // Latch on any failure / error. 
}

void NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    /* Enable the TIM2 gloabal Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

    NVIC_Init(&NVIC_InitStructure);

    /* Enable the TIM2 gloabal Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

    NVIC_Init(&NVIC_InitStructure);
}

int main(void)
{
    SystemInit();
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	NVIC_Configuration();
	
	eMBInit( MB_RTU, 0x01, 0, 9600, MB_PAR_NONE );
	
	/* Enable the Modbus Protocol Stack. */
	eMBEnable(  );
	
	for( ;; )
	{
	    ( void )eMBPoll(  );
	    /* Here we simply count the number of poll cycles. */
	}
}

