#include <main.h>

#include <stm32f4_discovery.h>

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include <stdio.h>


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


void vTask_citaj_tipku(void *pvParameters)
{
	uint8_t i=0;
	uint8_t gore=0;
	uint8_t dolje=0;
	uint8_t motor_gore=0;
	uint8_t motor_dolje=0;
	uint8_t motor_stop=0;
	uint8_t g[5]="gore";
	uint8_t d[6]="dolje";
	uint8_t s[5]="stop";
	while(1)
	{
		gore = GPIO_ReadInputDataBit(TIPKA_GORE_GPIOx, TIPKA_GORE_PinNumber);
		dolje= GPIO_ReadInputDataBit(TIPKA_DOLJE_GPIOx, TIPKA_DOLJE_PinNumber);
		
		if(gore==1)
		{
			motor_gore	=	1;
			motor_dolje	=	0;
			motor_stop	=	0;
		}
		else if(dolje==1)
		{
			motor_gore	=	0;
			motor_dolje	=	1;
			motor_stop	=	0;
		}
		else
		{
			motor_gore	=	0;
			motor_dolje	=	0;
			motor_stop	=	1;
		}	
		if(motor_gore) 
		{
				for(i=0;i<5;++i)
				{
					USART_SendData(USART2, g[i]);
					vTaskDelay(10 / portTICK_RATE_MS);
				}
				USART_SendData(USART2, ' ');
		}
		else if(motor_dolje)
		{
			for(i=0;i<6;++i)
				{
					USART_SendData(USART2, d[i]);
					vTaskDelay(10 / portTICK_RATE_MS);
				}
			USART_SendData(USART2, ' ');
		}
		else
		{
			for(i=0;i<5;++i)
			{
				USART_SendData(USART2, s[i]);
				vTaskDelay(10 / portTICK_RATE_MS);
			}
			USART_SendData(USART2, ' ');
		}
		
		vTaskDelay(100 / portTICK_RATE_MS);
	}
}

int main(void)
{
	gpio_init();
	USART1_Init();
	USART2_Init();
	// create task
	xTaskCreate(vTask_citaj_tipku, "TASK1", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
	// run task scheduler
	vTaskStartScheduler();
}