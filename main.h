#ifndef	MAIN_H
#define	MAIN_H

#include <stm32f4xx.h>					//common stuff
#include <stm32f4xx_gpio.h>			//gpio control
#include <stm32f4xx_rcc.h>			//reset and clocking
#include <stm32f4xx_usart.h>		//USART
#include <gpio.h>				
#include <usart.h>

// forward declarations of task functions 
void vTask1(void *pvParameters); 
void vTask2(void *pvParameters); 
void vTask3(void *pvParameters); 
void vTask4(void *pvParameters);
#endif