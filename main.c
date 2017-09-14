#include <stdio.h>
#include "stm32f4xx.h"
#include <mb.h>
#include <mbport.h>

#include <main.h>

#include <stm32f4_discovery.h>

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/* ----------------------- Defines ------------------------------------------*/
#define REG_INPUT_START                 ( 1000 )
#define REG_INPUT_NREGS                 ( 64 )

#define REG_HOLDING_START               ( 40001 )
#define REG_HOLDING_NREGS               ( 32 )

/* ----------------------- Static variables ---------------------------------*/
static USHORT   usRegInputStart = REG_INPUT_START;
static USHORT   usRegInputBuf[REG_INPUT_NREGS];
static USHORT   usRegHoldingStart = REG_HOLDING_START;
static USHORT   usRegHoldingBuf[REG_HOLDING_NREGS];

/* ----------------------- Static functions ---------------------------------*/
static void     SetupHardware( void );
//static void     vTaskApplication( void *pvArg );
static void 		vTask_Modbus( void *pvParameters );

xQueueHandle xQueue;


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

    /* Enable the TIM2 global Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

    NVIC_Init(&NVIC_InitStructure);

    /* Enable the USART1 global Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

    NVIC_Init(&NVIC_InitStructure);
}

void vTask_Modbus(void *pvParameters)
{
	USHORT motor_dolje=0;
	USHORT motor_gore=0;
	USHORT motor_stop=0;
	USHORT stari=0;
	while(1)
	{
	(void)eMBPoll();
		if(usRegHoldingBuf[0]==0xF && stari!=0xF)
		{
			//motor dolje
			stari=0xF;
			xQueueSendToBack(xQueue, &stari, 10/portTICK_RATE_MS);
			vTaskDelay(10/portTICK_RATE_MS);
		}
		else if(usRegHoldingBuf[0]==0xF0 && stari!=0xF0)
		{
			//motor gore
			stari=0xF0;
			xQueueSendToBack(xQueue, &stari, 10/portTICK_RATE_MS);
			vTaskDelay(10/portTICK_RATE_MS);
		}
		else if(usRegHoldingBuf[0]==0xFF && stari!=0xFF)
		{
			//motor stop
			stari=0xFF;
			xQueueSendToBack(xQueue, &stari, 10/portTICK_RATE_MS);
			vTaskDelay(10/portTICK_RATE_MS);
		}
	}
}

void vTask_citaj_tipku(void *pvParameters)
{
	USHORT gore=0;
	USHORT dolje=0;
	USHORT stari=0;
	portBASE_TYPE xStatus;
	
	while(1)
	{
		gore = GPIO_ReadInputDataBit(TIPKA_GORE_GPIOx, TIPKA_GORE_PinNumber);
		dolje= GPIO_ReadInputDataBit(TIPKA_DOLJE_GPIOx, TIPKA_DOLJE_PinNumber);
		
		if(gore==1)
		{
			if(stari!=0xF0)
			{
				stari=0xF0;
				xStatus = xQueueSendToBack(xQueue, &stari, 10/portTICK_RATE_MS);
				vTaskDelay(10/portTICK_RATE_MS);
			}
			//ako je razlicito od proslog stanja salji u queue = gore
		}
		else if(dolje==1)
		{
			if(stari!=0x0F)
			{
				stari=0x0F;
				xStatus = xQueueSendToBack(xQueue, &stari, 10/portTICK_RATE_MS);
				vTaskDelay(10/portTICK_RATE_MS);
			}
			//ako je razlicito od proslog stanja salji u queue = dolje
		}
		else
		{
			if(stari!=0xFF)
			{
				stari=0xFF;
				xStatus = xQueueSendToBack(xQueue, &stari, 10/portTICK_RATE_MS);
				vTaskDelay(10/portTICK_RATE_MS);
			}
			//ako je razlicito od proslog stanja salji u queue = stop
		}	
	}
}

void vTask_motor(void *pvParameters)
{
	USHORT stari = 0;
	USHORT novi = 0;
	portBASE_TYPE xStatus;
	
	const portTickType xTicksToWait = 10/portTICK_RATE_MS;
	
	while(1)
	{
		xQueueReceive(xQueue, &novi, xTicksToWait);
		if(novi!=stari)
		{
			stari=novi;
			if(stari!=0xFF)
			{
				gpio_led_state(LED5_RED_ID, 1);
				gpio_led_state(LED4_GREEN_ID, 0);
				gpio_led_state(LED6_BLUE_ID, 0);
				
				GPIO_WriteBit(MOTOR_DOLJE_GPIOx, MOTOR_DOLJE_PinNumber, 0);
				GPIO_WriteBit(MOTOR_GORE_GPIOx, MOTOR_GORE_PinNumber, 0);
				//motor stop
			}
			if(novi==0x0F)
			{
				gpio_led_state(LED6_BLUE_ID, 1);
				gpio_led_state(LED4_GREEN_ID, 0);
				gpio_led_state(LED5_RED_ID, 0);
				
				GPIO_WriteBit(MOTOR_GORE_GPIOx, MOTOR_GORE_PinNumber, 0);
				GPIO_WriteBit(MOTOR_DOLJE_GPIOx, MOTOR_DOLJE_PinNumber, 1);
				//motor dolje
			}
			else if(novi==0xF0)
			{
				gpio_led_state(LED4_GREEN_ID, 1);
				gpio_led_state(LED6_BLUE_ID, 0);
				gpio_led_state(LED5_RED_ID, 0);
				
				GPIO_WriteBit(MOTOR_DOLJE_GPIOx, MOTOR_DOLJE_PinNumber, 0);
				GPIO_WriteBit(MOTOR_GORE_GPIOx, MOTOR_GORE_PinNumber, 1);
				//motor gore
			}
			else if(novi==0xFF)
			{
				gpio_led_state(LED5_RED_ID, 1);
				gpio_led_state(LED4_GREEN_ID, 0);
				gpio_led_state(LED6_BLUE_ID, 0);
				
				GPIO_WriteBit(MOTOR_DOLJE_GPIOx, MOTOR_DOLJE_PinNumber, 0);
				GPIO_WriteBit(MOTOR_GORE_GPIOx, MOTOR_GORE_PinNumber, 0);
				//motor stop
			}
		}
		vTaskDelay(10/portTICK_RATE_MS);
	}
}
	

int main(void)
{
  SystemInit();
	SetupHardware();
	NVIC_Configuration();
	gpio_init();
	USART2_Init();
	eMBErrorCode    eStatus;
	
	xQueue = xQueueCreate(5, sizeof(short));
	
	eMBInit( MB_RTU, 0x01, 0, 9600, MB_PAR_NONE );
	
	//usRegHoldingBuf[0]=10;
	
	/* Enable the Modbus Protocol Stack. */
	eMBEnable(  );
	
	xTaskCreate(vTask_Modbus, "TASK_MODBUS", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
	xTaskCreate(vTask_citaj_tipku, "TASK_TIPKA", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
	xTaskCreate(vTask_motor, "TASK_MOTOR", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
	vTaskStartScheduler();
	while(1);
}

static void
SetupHardware( void )
{
    //WDT_Disable(  );

    uint32_t        i = 0;
    for( i = 0; i < 35; i++ )
    {
        NVIC_SetPriority( ( IRQn_Type ) i, 0xF << 4 );
    }
}

eMBErrorCode
eMBRegInputCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs )
{
    eMBErrorCode    eStatus = MB_ENOERR;
    int             iRegIndex;

    if( ( usAddress >= REG_INPUT_START ) && ( usAddress + usNRegs <= REG_INPUT_START + REG_INPUT_NREGS ) )
    {
        iRegIndex = ( int )( usAddress - usRegInputStart );
        while( usNRegs > 0 )
        {
            *pucRegBuffer++ = ( unsigned char )( usRegInputBuf[iRegIndex] >> 8 );
            *pucRegBuffer++ = ( unsigned char )( usRegInputBuf[iRegIndex] & 0xFF );
            iRegIndex++;
            usNRegs--;
        }
    }
    else
    {
        eStatus = MB_ENOREG;
    }

    return eStatus;
}

eMBErrorCode
eMBRegHoldingCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs, eMBRegisterMode eMode )
{
    eMBErrorCode    eStatus = MB_ENOERR;
    int             iRegIndex;

    if( ( usAddress >= REG_HOLDING_START ) && ( usAddress + usNRegs <= REG_HOLDING_START + REG_HOLDING_NREGS ) )
    {
        iRegIndex = ( int )( usAddress - usRegHoldingStart );
        switch ( eMode )
        {
        case MB_REG_READ:
            while( usNRegs > 0 )
            {
                *pucRegBuffer++ = ( unsigned char )( usRegHoldingBuf[iRegIndex] >> 8 );
                *pucRegBuffer++ = ( unsigned char )( usRegHoldingBuf[iRegIndex] & 0xFF );
                iRegIndex++;
                usNRegs--;
            }
            break;

        case MB_REG_WRITE:
            while( usNRegs > 0 )
            {
                usRegHoldingBuf[iRegIndex] = *pucRegBuffer++ << 8;
                usRegHoldingBuf[iRegIndex] |= *pucRegBuffer++;
                iRegIndex++;
                usNRegs--;
            }
        }
    }
    else
    {
        eStatus = MB_ENOREG;
    }
    return eStatus;
}


eMBErrorCode
eMBRegCoilsCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNCoils, eMBRegisterMode eMode )
{
    return MB_ENOREG;
}

eMBErrorCode
eMBRegDiscreteCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNDiscrete )
{
    return MB_ENOREG;
}

